#include <ace/Null_Mutex.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Guard_T.h>
//local includes
#include "xglog.h"
#include "xGateHmpProcessor.h"
#include "xGateMGDispatcher.h"
#include "xGateHmpGstManager.h"
#include "xGateUtil.h"
#include "xGateMgMsg.h"

#define THISMODULE "HmpProc"
#define ENABLE_MRCP 0
#define ENABLE_SMATIC 1

xGateHmpProcessor::xGateHmpProcessor(const char *serverType)
	 :xGateProcessor(serverType),m_pmgMsg(NULL)
{
	 XGLOG_INFO( "xGateHmpProcessor::xGateHmpProcessor");
}

xGateHmpProcessor::~xGateHmpProcessor(void)
{
	 XGLOG_INFO( "xGateHmpProcessor::~xGateHmpProcessor");
}

xGateRetVal xGateHmpProcessor::initMRCPBridge()
{
	 if(!UR_MRCP_MODULE_BRIDGE::instance()->init())
	 {
			cout<<"URMRCPModuleBridge init() failed"<<endl;
	 }
	 return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpProcessor::process_msg(xGateBaseMsg* pMsg)
{
	 XGLOG_INFO("Enter xGateHmpProcessor::process_msg start (%s)",(char*)ur_log_string(m_serverType)); 
	 xGateRetVal retVal = EN_XGATE_STATUS_ERROR;

	 XGLOG_INFO("The xGateHmpProcessor:::process_msg start for pMgMsg->get_mg_msg_type (%d)",pMsg->getMsgType());

	 switch(pMsg->getMsgType()) {
			case EN_XGATE_MSG_MGC_MG_IN:
				 {
						XGLOG_INFO( "xGateHmpProcessor::process_msg (processing EN_XGATE_MSG_MGC_MG_IN message)"); 
						xGateMgMsg *pMgMsg = dynamic_cast<xGateMgMsg*>(pMsg);
						MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
						xGateMgMsgType msgType;
						msgType=pMgMsg->get_mg_msg_type();

						//TODO: handle channel request from client
						XGLOG_INFO( "xGateHmpProcessor::process_msg handle_stt_tts_channel_req");
						retVal = handle_stt_tts_channel_req(pMgMsg);
						if(retVal == EN_XGATE_STATUS_SUCCESS) {
							 post_media_detail(pMgMsg);
						} else if(retVal == EN_XGATE_STATUS_ERROR) {
							 XGLOG_ERROR( "Enter xGateHmpProcessor::process_msg handle_stt_tts_channel_req failed");
						}
						break;
				 }
			case EN_XGATE_MSG_MGC_MG_OUT:
				 {
						//at present we don't need to support this from HmpProcessor thread.
						XGLOG_INFO( "xGateHmpProcessor::process_msg (processing EN_XGATE_MSG_MGC_MG_OUT message)");
						break;
				 }
			case EN_XGATE_MSG_STT_TTS_IN:
				 {
						xGateMRCPMsg *pServicemsg = dynamic_cast<xGateMRCPMsg*>(pMsg);
						printf("####### GET--->EN_XGATE_MSG_STT_TTS_IN ########\n");
						XGLOG_INFO("####### GET--->EN_XGATE_MSG_STT_TTS_IN ########");
						retVal=handle_stt_tts_msg(pServicemsg);
						if(retVal != EN_XGATE_STATUS_SUCCESS){
							 XGLOG_ERROR(" xGateHmpProcessor::process_msg handle_stt_tts_msg failed");
						}
						break;
				 }
#ifdef XGATE_HAS_SELF_AUDIT
			case EN_XGATE_MSG_AUDIT:
				 {
						xGateAuditMsg *pAuditMsg = dynamic_cast<xGateAuditMsg*>(pMsg);
						XGLOG_INFO( "xGateHmpProcessor::process_msg (processing EN_XGATE_MSG_AUDIT message)");
						//TODO: Yoga, need to implement this part
						retVal = handle_audit_msg(pAuditMsg);
						break;
				 }
#endif
			case EN_XGATE_MSG_UNKNOWN:
			default:
				 {
						XGLOG_ERROR( "Enter xGateHmpProcessor::process_msg is EN_XGATE_MSG_UNKNOWN");
						break;
				 }      
	 }

	 cout<<"The xGateHmpProcessor::process_msg "<< pMsg->getMsgType()<<endl;
	 XGLOG_INFO("The xGateHmpProcessor:::process_msg End for pMgMsg->get_mg_msg_type (%d) End",pMsg->getMsgType());

	 return EN_XGATE_STATUS_SUCCESS; 
}

xGateRetVal xGateHmpProcessor::handle_stt_tts_channel_req(xGateMgMsg* pMgMsg) 
{
	 xGateHmpGstManager *pHmpGstManager = xGateUtil::getHmpGstManager();
	 MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();

	 XGLOG_INFO("xGateHmpProcessor::handle_stt_tts_channel_req start for %s",(char*)ur_log_string(mediaDetail.mgresource_id));

	 if(!pHmpGstManager) {
			XGLOG_ERROR( "xGateHmpProcessor::handle_stt_tts_channel_req failed for %s. Invalid HmpGstManager object !!",(char*)ur_log_string(mediaDetail.mgresource_id));
			return EN_XGATE_STATUS_ERROR;
	 }


	 xGateNetConnection pbxConn; 
	 pMgMsg->get_net_con_info(pbxConn);
	 switch(pMgMsg->get_mg_msg_type()) {
			case EN_MEDIA_STT_TTS_PORT_ALLOCATE:   //Invite Received with SDP from Leg-A  [Msg-23]
				 {
						XGLOG_INFO("handle_stt_tts_channel_req for %s processing EN_MEDIA_STT_TTS_PORT_ALLOCATE request",(char*)ur_log_string(mediaDetail.mgresource_id));
						handle_media_event(pMgMsg);
						return EN_XGATE_STATUS_NOT_SUPPORTED;
				 }
			case EN_XGATE_STT_TTS_RELEASE:   
				 {
						XGLOG_INFO( "handle_stt_tts_channel_req for %s processing EN_XGATE_STT_TTS_RELEASE request",(char*)ur_log_string(mediaDetail.mgresource_id));
						pHmpGstManager->release_stt_tts_channel(mediaDetail);
						xGateDbConfigMgrServiceMsg *Servicemsg =new xGateDbConfigMgrServiceMsg();
						Servicemsg->set_mrcp_command(EN_MSG_RELEASE);
						Servicemsg->m_callId = mediaDetail.call_id;
						Servicemsg->m_uid = mediaDetail.mgresource_id;
						PostMessageToMrcpClient(Servicemsg);
						return EN_XGATE_STATUS_NOT_SUPPORTED;
						break;
				 }
			case EN_XGATE_STT_TTS_REMOVE_PARTICIPANT:
				 {
						XGLOG_INFO( "handle_stt_tts_channel_req for %s processing EN_XGATE_STT_TTS_REMOVE_PARTICIPANT",(char*)ur_log_string(mediaDetail.mgresource_id));
						pHmpGstManager->media_delete_channel(mediaDetail);
						xGateDbConfigMgrServiceMsg *Servicemsg =new xGateDbConfigMgrServiceMsg();
						Servicemsg->set_mrcp_command(EN_STT_REMOVE_PARTICIPANT);
						Servicemsg->m_callId = mediaDetail.call_id;
						Servicemsg->m_uid = mediaDetail.mgresource_id;
						PostMessageToMrcpClient(Servicemsg);
						return EN_XGATE_STATUS_NOT_SUPPORTED;
				 } 
			case EN_XGATE_STT_TTS_TRANSFER:
				 {
						XGLOG_INFO( "handle_stt_tts_channel_req for %s processing EN_XGATE_STT_TTS_TRANSFER",(char*)ur_log_string(mediaDetail.mgresource_id));
						xGateDbConfigMgrServiceMsg  *Servicemsg = new xGateDbConfigMgrServiceMsg();
						Servicemsg->set_mrcp_command(EN_STT_TTS_TRANSFER_ADD_PARTICIPANT);
						Servicemsg->m_callId = mediaDetail.call_id;
						Servicemsg->m_uid = mediaDetail.mgresource_id;
						PostMessageToMrcpClient(Servicemsg);
						pHmpGstManager->handle_call_transfer(mediaDetail);
						break;
				 }
			case EN_XGATE_STT_TTS_MEDIA_CREATE: // Need to check the functionality when conference involves 
				 {
						XGLOG_INFO( "handle_stt_tts_channel_req for %s processing EN_XGATE_STT_TTS_MEDIA_CREATE",(char*)ur_log_string(mediaDetail.mgresource_id));
						g_print( "handle_stt_tts_channel_req processing EN_XGATE_STT_TTS_MEDIA_CREATE request ,Inband_Dtmf:%d\n",mediaDetail.audioDetail.m_inBandDtmf);
						pHmpGstManager->allocate_stt_tts_channel(mediaDetail, pbxConn); //conference channel allocation
						break;
				 }
			case EN_XGATE_STT_TTS_RECONNECTION:
				 {
						XGLOG_INFO( "handle_stt_tts_channel_req for %s processing EN_XGATE_STT_TTS_RECONNECTION",(char*)ur_log_string(mediaDetail.mgresource_id));
						pHmpGstManager->reallocate_stt_tts_channel(mediaDetail, pbxConn); //conference channel allocation
						xGateDbConfigMgrServiceMsg  *Servicemsg = new xGateDbConfigMgrServiceMsg();
						//Servicemsg->set_mrcp_command(EN_STT_TTS_RECONNECTION_MSG);
						PostMessageToMrcpClient(Servicemsg);
						break;
				 }
			case EN_XGATE_STT_TTS_PLAY_TEXT:
				 {
						XGLOG_INFO( "handle_stt_tts_channel_req for %s processing EN_XGATE_MRCP_PLAY_TEXT",(char*)ur_log_string(mediaDetail.mgresource_id));
						xGateDbConfigMgrServiceMsg *serviceMsg = new xGateDbConfigMgrServiceMsg();
						serviceMsg->set_mrcp_command(EN_MRCP_MSG_SYNTHESIZE_REQ);
						serviceMsg->m_callId = mediaDetail.call_id;
						serviceMsg->m_uid = mediaDetail.mgresource_id;
						serviceMsg->m_displayText = mediaDetail.tts_text.c_str();
						serviceMsg->set_chatbot_init(mediaDetail.ccaas_chatbot_init);
						serviceMsg->m_bisBotFirst = mediaDetail.ccaas_bot_first;
						serviceMsg->m_strCcaasBlockId = mediaDetail.ccaas_block_id.c_str();
						serviceMsg->m_strCcaasBlockType = mediaDetail.ccaas_block_type.c_str();
						serviceMsg->m_strCcaasChannelId= mediaDetail.ccaas_channel_id.c_str();
						serviceMsg->m_strCcaasAgentId = mediaDetail.ccaas_agent_id.c_str();
						serviceMsg->m_strCcaasBotUrl = mediaDetail.ccaas_bot_url.c_str();
						serviceMsg->m_iCcaasBotId = mediaDetail.ccaas_bot_id;
						PostMessageToMrcpClient(serviceMsg);
						return EN_XGATE_STATUS_NOT_SUPPORTED;
				 }
			case EN_XGATE_MG_TYPE_UNKNOWN:
			default:
				 {
						XGLOG_ERROR( "handle_stt_tts_channel_req failed for %s. Not able to process MG_TYPE_UNKNOWN request !!",(char*)ur_log_string(mediaDetail.mgresource_id));
						return EN_XGATE_STATUS_NOT_SUPPORTED;
						break;
				 }
	 }

	 XGLOG_INFO( "handle_stt_tts_channel_req End for %s",(char*)ur_log_string(mediaDetail.mgresource_id));

	 return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpProcessor::handle_media_event(xGateMgMsg *pMgMsg)
{
	 MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
	 xGateMgMsgType msgType;
	 msgType=pMgMsg->get_mg_msg_type();

	 string callId = mediaDetail.call_id;
	 string mgresourceId = mediaDetail.mgresource_id;
	 xGateCallType callType = mediaDetail.call_type;
	 XGLOG_INFO("xGateHmpProcessor::handle_media_event for mgresource_id:%s call_id: %s start", \
				 (char*)ur_log_string(mgresourceId), (char*)ur_log_string(callId));

	 xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
	 STT_TTS_STATE mrcpState = STT_TTS_STATE::EN_STT_TTS_STATE_EVENT_NULL;
	 xGateMrcpCommand mrcpCommand = EN_MSG_UNKNOWN;

	 xGateHmpGstManager *pHmpGstManager = xGateUtil::getHmpGstManager();
	 if(!pHmpGstManager) {
			XGLOG_ERROR( "handle_channel_req failed. Invalid HmpGstManager object !!");
			return EN_XGATE_STATUS_ERROR;
	 }

	 xGateNetConnection pbxConn;
	 pMgMsg->get_net_con_info(pbxConn);

	 mrcpState = mediaDetail.mrcp_state;

	 XGLOG_INFO("xGateHmpProcessor::PROCESS mrcp state: %d starts for the callID: %s ", mrcpState, (char*)ur_log_string(callId));

	 switch(mrcpState)
	 {
			case STT_TTS_STATE::EN_TTS_START:
				 {
						XGLOG_INFO("xGateHmpProcessor::PROCESS------->STT_TTS_STATE::EN_TTS_START:%s", (char*)ur_log_string(callId));
						xGateDbConfigMgrServiceMsg *Servicemsg =new xGateDbConfigMgrServiceMsg();
						Servicemsg->set_mrcp_command(EN_TTS_MSG_START_REQ);
						Servicemsg->m_callId=mediaDetail.call_id;
						Servicemsg->m_uid=mediaDetail.mgresource_id;
						PostMessageToMrcpClient(Servicemsg);
						handle_pipeline_start_request(mediaDetail.call_id,mediaDetail.mgresource_id,mrcpState,mrcpCommand);
						cout<<"EN_TTS_START!!!!!!!!!!!!!!!!!"<<endl;
						break;
				 }
			case STT_TTS_STATE::EN_STT_ALLOCATE_PORT:
				 {
						XGLOG_INFO("xGateHmpProcessor::PROCESS------->EN_STT_ALLOCATE_PORT:%s", (char*)ur_log_string(callId));
						pHmpGstManager->allocate_stt_tts_channel(mediaDetail, pbxConn);
						xGateDbConfigMgrServiceMsg *smaticServicemsg =new xGateDbConfigMgrServiceMsg();
						smaticServicemsg->set_mrcp_command(EN_MSG_PORT_ALLOCATE_REQ);
						smaticServicemsg->m_callId = mediaDetail.call_id;
						smaticServicemsg->m_uid = mediaDetail.mgresource_id;
						string session=encode_stt_tts_sdp_message(mediaDetail);
						smaticServicemsg->set_mrcp_session_detail(session.c_str());
						smaticServicemsg->set_asr_type(mediaDetail.asr_type);
						mediaDetail.mrcp_port = to_string(mediaDetail.audioDetail.m_serverPort);
						mediaDetail.mrcp_ip_info = mediaDetail.audioDetail.m_serverIp;
						PostMessageToMrcpClient(smaticServicemsg);
						break;
				 }
			case STT_TTS_STATE::EN_STT_TTS_ALLOCATE_PORT: //SMT
				 {
						XGLOG_INFO("xGateHmpProcessor::PROCESS------->EN_STT_TTS_ALLOCATE_PORT:%s", (char*)ur_log_string(callId));
						pHmpGstManager->allocate_stt_tts_channel(mediaDetail, pbxConn);
						xGateDbConfigMgrServiceMsg *smaticServicemsg =new xGateDbConfigMgrServiceMsg();
						smaticServicemsg->set_mrcp_command(EN_MRCP_MSG_ALLOCATE_STT_TTS);
						string session=encode_stt_tts_sdp_message(mediaDetail);
						smaticServicemsg->set_asr_type(mediaDetail.asr_type);
						cout<<"The asr type came from PBX sever "<<mediaDetail.asr_type<<endl;
						// smaticServicemsg->set_asr_type(EN_XGATE_ASR_INHOUSE);
						smaticServicemsg->set_tts_type(mediaDetail.tts_type);
						cout<<"The tts type came from PBX sever !!!!!"<<mediaDetail.tts_type<<endl;
						//smaticServicemsg->set_tts_type(EN_XGATE_TTS_INHOUSE);
						// smaticServicemsg->set_tts_type(EN_XGATE_TTS_READ_SPEAKER);
						smaticServicemsg->set_mrcp_session_detail(session.c_str());
						mediaDetail.mrcp_port = to_string(mediaDetail.audioDetail.m_serverPort);
						mediaDetail.ccaas_voice_recog_port = mediaDetail.audioDetail.m_server64Port;
						mediaDetail.mrcp_ip_info = mediaDetail.audioDetail.m_serverIp;
						PostMessageToMrcpClient(smaticServicemsg);
						handle_pipeline_start_request(mediaDetail.call_id,mediaDetail.mgresource_id,mrcpState,mrcpCommand);
						break;
				 }

			case STT_TTS_STATE::EN_STT_START:
			case STT_TTS_STATE::EN_STT_GET_VARIABLE: //SMT
				 {
						XGLOG_INFO("xGateHmpProcessor::handle_media_event, EN_MRCP_STT_START | EN_STT_GET_VARIABLE for mgResourceId:%s,CallId:%s",(char*)ur_log_string(mediaDetail.mgresource_id),\
									(char*)ur_log_string(mediaDetail.call_id));
						switch(mediaDetail.asr_type)
						{
							 case EN_XGATE_ASR_SPEECH_MATICS:
							 case EN_XGATE_ASR_INHOUSE:
									{
										 XGLOG_INFO("xGateHmpProcessor::handle_media_event, EN_MRCP_STT_START | EN_STT_GET_VARIABLE choosing 'EN_XGATE_ASR_SPEECH_MATICS | EN_XGATE_ASR_INHOUSE' for mgResourceId:%s,CallId:%s ",\
													 (char*)ur_log_string(mediaDetail.mgresource_id),(char*)ur_log_string(mediaDetail.call_id));
										 pHmpGstManager->handle_stt_start_request(mediaDetail.mgresource_id,mediaDetail.call_id);
										 break;
									}
							 default:
									{
										 XGLOG_INFO("xGateHmpProcessor::handle_media_event, EN_MRCP_STT_START | EN_STT_GET_VARIABLE choosing 'EN_XGATE_ASR_TYPE_UNKNOWN' for mgResourceId:%s,CallId:%s ",\
													 (char*)ur_log_string(mediaDetail.mgresource_id),(char*)ur_log_string(mediaDetail.call_id));
										 break;
									}
						}
						if(mrcpState == STT_TTS_STATE::EN_STT_GET_VARIABLE || mrcpState == STT_TTS_STATE::EN_STT_START)
						{
							 xGateDbConfigMgrServiceMsg *smaticServicemsg =new xGateDbConfigMgrServiceMsg();
							 smaticServicemsg->set_mrcp_command((mrcpState==STT_TTS_STATE::EN_STT_GET_VARIABLE) ? EN_GET_VARAIBLE_STT : (mrcpState == STT_TTS_STATE::EN_STT_START) ? EN_STT_START : EN_MSG_UNKNOWN);
							 smaticServicemsg->set_call_id(mediaDetail.call_id.c_str());
							 PostMessageToMrcpClient(smaticServicemsg);
						}
						break;
				 }
			case STT_TTS_STATE::EN_STT_STOP:
				 {
						XGLOG_INFO("xGateHmpProcessor::handle_media_event, EN_MRCP_STT_STOP for mgResourceId:%s,CallId:%s",(char*)ur_log_string(mediaDetail.mgresource_id),\
									(char*)ur_log_string(mediaDetail.call_id));
						switch(mediaDetail.asr_type)
						{
							 case EN_XGATE_ASR_SPEECH_MATICS:
									{
										 XGLOG_INFO("xGateHmpProcessor::handle_media_event, EN_MRCP_STT_STOP choosing 'EN_XGATE_ASR_SPEECH_MATICS' for mgResourceId:%s,CallId:%s ",\
													 (char*)ur_log_string(mediaDetail.mgresource_id),(char*)ur_log_string(mediaDetail.call_id));
										 handle_pipeline_stop_request(mediaDetail.call_id,mediaDetail.mgresource_id,mrcpState,mrcpCommand);
										 break;
									}
							 case EN_XGATE_ASR_INHOUSE:
									{
										 XGLOG_INFO("xGateHmpProcessor::handle_media_event, EN_MRCP_STT_STOP choosing 'EN_XGATE_ASR_INHOUSE' for mgResourceId:%s,CallId:%s ",(char*)ur_log_string(mediaDetail.mgresource_id),(char*)ur_log_string(mediaDetail.call_id));
										 handle_pipeline_stop_request(mediaDetail.call_id,mediaDetail.mgresource_id,mrcpState,mrcpCommand);
										 xGateDbConfigMgrServiceMsg *Servicemsg =new xGateDbConfigMgrServiceMsg();
										 Servicemsg->set_mrcp_command(EN_MRCP_MSG_RECOGNITION_STOP);
										 Servicemsg->set_asr_type(mediaDetail.asr_type);
										 Servicemsg->m_callId=mediaDetail.call_id;
										 Servicemsg->m_uid=mediaDetail.mgresource_id;
										 PostMessageToMrcpClient(Servicemsg);
										 break;
									}
							 default:
									{
										 XGLOG_INFO("xGateHmpProcessor::handle_media_event, EN_MRCP_STT_STOP choosing 'EN_XGATE_ASR_TYPE_UNKNOWN' for mgResourceId:%s,CallId:%s ",\
													 (char*)ur_log_string(mediaDetail.mgresource_id),(char*)ur_log_string(mediaDetail.call_id));
										 break;
									}
						}
						break;
				 }
			case STT_TTS_STATE::EN_CCAAS_VOICE_RECOG_START:
				 {
						XGLOG_INFO("xGateHmpProcessor::handle_media_event, 'EN_CCAAS_VOICE_RECOG_START' for mgResourceId:%s,CallId:%s ",\
									(char*)ur_log_string(mediaDetail.mgresource_id),(char*)ur_log_string(mediaDetail.call_id));
						handle_base64_encoding_start_request(mediaDetail.call_id,mediaDetail.mgresource_id);
						break;
				 }

			case STT_TTS_STATE::EN_CCAAS_VOICE_RECOG_STOP:
				 {
						XGLOG_INFO("xGateHmpProcessor::handle_media_event, 'EN_CCAAS_VOICE_RECOG_STOP' for mgResourceId:%s,CallId:%s ",(char*)ur_log_string(mediaDetail.mgresource_id),\
									(char*)ur_log_string(mediaDetail.call_id));
						handle_base64_encoding_stop_request(mediaDetail.call_id,mediaDetail.mgresource_id);
						break;
				 }
			case STT_TTS_STATE::EN_TTS_STOP_REQ:
				 {
						XGLOG_INFO("xGateHmpProcessor::handle_media_event, 'EN_TTS_STOP_REQ' for mgResourceId:%s,CallId:%s ",(char*)ur_log_string(mediaDetail.mgresource_id),\
									(char*)ur_log_string(mediaDetail.call_id));
						xGateDbConfigMgrServiceMsg *Servicemsg =new xGateDbConfigMgrServiceMsg();
						Servicemsg->set_mrcp_command(EN_MRCP_MSG_SYNTHESIZE_STOP);
						Servicemsg->m_callId=mediaDetail.call_id;
						Servicemsg->m_uid=mediaDetail.mgresource_id;
						PostMessageToMrcpClient(Servicemsg);
						break;
				 }
			case STT_TTS_STATE::EN_STT_RECOGNITION_START:
				 {
						XGLOG_INFO("xGateHmpProcessor::PROCESS------->EN_STT_RECOGNITION_START for CallId:%s",(char*)ur_log_string(callId));
						pHmpGstManager->start_stt_capture_channel(mediaDetail);
						xGateDbConfigMgrServiceMsg *Servicemsg =new xGateDbConfigMgrServiceMsg();
						Servicemsg->m_callId=mediaDetail.call_id;
						Servicemsg->m_uid=mediaDetail.mgresource_id;
						Servicemsg->set_mrcp_command(EN_STT_START_REQ);
						Servicemsg->set_stt_enabled(mediaDetail.mrcp_isSttEnabled);
						string session=encode_stt_tts_sdp_message(mediaDetail);
            Servicemsg->set_mrcp_session_detail(session.c_str());
						PostMessageToMrcpClient(Servicemsg);
						return EN_XGATE_STATUS_NOT_SUPPORTED;
						break;
				 }
			case STT_TTS_STATE::EN_STT_RECOGNITION_PAUSE:
				 {
						XGLOG_INFO("xGateHmpProcessor::PROCESS------->EN_STT_RECOGNITION_PAUSE for CallId:%s ",(char*)ur_log_string(mediaDetail.call_id));
						xGateDbConfigMgrServiceMsg *Servicemsg =new xGateDbConfigMgrServiceMsg();
						Servicemsg->m_callId=mediaDetail.call_id;
						Servicemsg->m_uid=mediaDetail.mgresource_id;
						Servicemsg->set_mrcp_command(EN_STT_PAUSE_REQ);
						Servicemsg->set_stt_enabled(mediaDetail.mrcp_isSttEnabled);
						PostMessageToMrcpClient(Servicemsg);
						return EN_XGATE_STATUS_NOT_SUPPORTED;
						break;
				 }
			case STT_TTS_STATE::EN_STT_RECOGNITION_RESUME:
				 {
						XGLOG_INFO("xGateHmpProcessor::PROCESS-------> EN_STT_RECOGNITION_RESUME for CallId:%s ",(char*)ur_log_string(mediaDetail.call_id));
						xGateDbConfigMgrServiceMsg *Servicemsg =new xGateDbConfigMgrServiceMsg();
						pHmpGstManager->start_stt_capture_channel(mediaDetail);
						Servicemsg->m_callId=mediaDetail.call_id;
						Servicemsg->m_uid=mediaDetail.mgresource_id;
						Servicemsg->set_mrcp_command(EN_STT_RESUME_REQ);
						Servicemsg->set_stt_enabled(mediaDetail.mrcp_isSttEnabled);
						PostMessageToMrcpClient(Servicemsg);
						return EN_XGATE_STATUS_NOT_SUPPORTED;
						break;
				 }
			case STT_TTS_STATE::EN_STT_RECOGNITION_STOP:
				 {
						XGLOG_INFO("xGateHmpProcessor::PROCESS------->EN_MRCP_RECOGNITION_STOP for CallId:%s",(char*)ur_log_string(mediaDetail.call_id));
						pHmpGstManager->stop_stt_capture_channel(mediaDetail);
						xGateDbConfigMgrServiceMsg *Servicemsg =new xGateDbConfigMgrServiceMsg();
						Servicemsg->m_callId=mediaDetail.call_id;
						Servicemsg->m_uid=mediaDetail.mgresource_id;
						Servicemsg->set_mrcp_command(EN_STT_STOP_REQ);
						Servicemsg->set_stt_enabled(mediaDetail.mrcp_isSttEnabled);
						PostMessageToMrcpClient(Servicemsg);
						return EN_XGATE_STATUS_NOT_SUPPORTED;
						break;
				 }
			case STT_TTS_STATE::EN_BOT_DEINIT:
				 {
						XGLOG_INFO("xGateHmpProcessor::handle_media_event, EN_BOT_DEINIT for mgResourceId:%s,CallId:%s ",(char*)ur_log_string(mediaDetail.mgresource_id),(char*)ur_log_string(mediaDetail.call_id));
						xGateDbConfigMgrServiceMsg *Servicemsg =new xGateDbConfigMgrServiceMsg();
						Servicemsg->set_mrcp_command(EN_VOICE_BOT_DEINIT);
						Servicemsg->set_asr_type(mediaDetail.asr_type);
						Servicemsg->m_callId=mediaDetail.call_id;
						Servicemsg->m_uid=mediaDetail.mgresource_id;
						PostMessageToMrcpClient(Servicemsg);
						break;
				 }
			case STT_TTS_STATE::EN_STT_TTS_STATE_EVENT_NULL:
			default:
				 {
						XGLOG_ERROR("xGateHmpProcessor::handle_media_event,EN_MRCP_MSG_UNKNOWN for mgResourceId:%s,CallId:%s ",(char*)ur_log_string(mediaDetail.mgresource_id),\
									(char*)ur_log_string(mediaDetail.call_id));
						break;
				 }
	 }

	 XGLOG_INFO("xGateHmpProcessor::PROCESS mrcp state: %d End for the callID: %s ", mrcpState, (char*)ur_log_string(callId));

	 return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpProcessor::handle_stt_tts_msg(xGateMRCPMsg *pServicemsg){

	 STT_TTS_STATE mrcpState = STT_TTS_STATE::EN_STT_TTS_STATE_EVENT_NULL;
	 xGateMrcpCommand msg=pServicemsg->get_mrcp_command();
	 string call_id=pServicemsg->get_mrcp_call_id();
	 string msg_id =pServicemsg->get_mrcp_uid();

	 XGLOG_INFO( "xGateHmpProcessor::handle_stt_tts_msg start for msgId:%s,CallId:%s",msg_id.c_str(), call_id.c_str());
	 xGateRetVal ret=EN_XGATE_STATUS_ERROR;
	 xGateHmpGstManager *pHmpGstManager = xGateUtil::getHmpGstManager();
	 if(!pHmpGstManager) {
			XGLOG_ERROR( "handle_channel_req failed. Invalid HmpGstManager object !!");
			return EN_XGATE_STATUS_ERROR;
	 }


	 if( call_id.empty() || msg_id.empty())
	 {
			XGLOG_ERROR("xGateHmpProcessor::handle_matic_msg,callID/mgID is empty !!!");
			return ret;
	 }
	 switch(msg){
			case EN_STT_MSG_RECOGNITION_START_REQ:
				 {
						XGLOG_INFO("xGateHmpProcessor::handle_stt_tts_msg, EN_STT_MSG_RECOGNITION_START_REQ for msgId:%s,CallId:%s",msg_id.c_str(), call_id.c_str());
						pHmpGstManager->handle_stt_start_request(msg_id,call_id);
						break;
				 }
			case EN_MSG_TRANSFER_REQ:
				 {
						string transferNum="";
						transferNum=pServicemsg->get_transfer_to_number();
						if(!transferNum.empty())
							 XGLOG_INFO("xGateHmpProcessor::handle_stt_tts_msg,EN_MSG_TRANSFER for msgId:%s,CallId:%s,TansferNumber:%s",\
										 msg_id.c_str(),call_id.c_str(),transferNum.c_str());
						pHmpGstManager->post_transfer_msg(msg_id,call_id,transferNum);
						break;
				 }
			case EN_STT_TTS_MSG_ALLOCATE_RES:
				 {
						XGLOG_INFO("xGateHmpProcessor::handle_stt_tts_msg, EN_STT_TTS_MSG_ALLOCATE_RES for msgId:%s,CallId:%s",msg_id.c_str(),call_id.c_str());
						pHmpGstManager->post_stt_tts_alloc_res_msg(msg_id,call_id);
						break;
				 }
			case EN_TTS_SPEAK_COMPLETE:
				 {
						XGLOG_INFO("xGateHmpProcessor::handle_stt_tts_msg, EN_TTS_SPEAK_COMPLETE for msgId:%s,CallId:%s",msg_id.c_str(),call_id.c_str());
						pHmpGstManager->post_tts_complete_msg(msg_id,call_id);
						break;
				 }
			case EN_TTS_CHANNEL_START_REQ:
				 {
						XGLOG_INFO("xGateHmpProcessor::handle_stt_tts_msg, EN_TTS_CHANNEL_START_REQ for msgId:%s,CallId:%s",msg_id.c_str(),call_id.c_str());
						// handle_pipeline_start_request(call_id,msg_id,mrcpState,msg);
						break;
				 }
			case EN_TTS_CHANNEL_STOP_REQ:
				 {
						XGLOG_INFO("xGateHmpProcessor::handle_stt_tts_msg, EN_TTS_CHANNEL_STOP_REQ for msgId:%s,CallId:%s",msg_id.c_str(),call_id.c_str());
						// handle_pipeline_stop_request(call_id,msg_id,mrcpState,msg);
						break;
				 }
			case EN_RESTART_STT_RECOGNIZATION:
				 {
						XGLOG_INFO("xGateHmpProcessor::handle_msg.EN_RESTART_STT_RECOGNIZATION for uId:%s,callId:%s",msg_id.c_str(),call_id.c_str());
						pHmpGstManager->handle_restart_stt_recognition(msg_id,call_id);      
						break;
				 }
      case EN_CREATE_STT_SOCKET_CONNECTION:
         {
            XGLOG_INFO("xGateHmpProcessor::handle_msg.EN_CREATE_STT_SOCKET_CONNECTION for uId:%s,callId:%s",msg_id.c_str(),call_id.c_str());
            pHmpGstManager->handle_create_stt_socket_connection(msg_id,call_id);
            break;
         }
      case EN_STT_RECOGNITION_PAUSE:
         {   
            XGLOG_INFO("xGateHmpProcessor::handle_msg.EN_STT_RECOGNITION_PAUSE for uId:%s,callId:%s",msg_id.c_str(),call_id.c_str()); 
            pHmpGstManager->handle_stt_recognition_pause_resume(msg_id,call_id,FALSE);
            break;
         }
      case EN_STT_RECOGNITION_RESUME:
         {
            XGLOG_INFO("xGateHmpProcessor::handle_msg.EN_STT_RECOGNITION_RESUME for uId:%s,callId:%s",msg_id.c_str(),call_id.c_str());
            pHmpGstManager->handle_stt_recognition_pause_resume(msg_id,call_id,TRUE);
            break;
         }
			case EN_MSG_UNKNOWN:
			default:
				 {
						XGLOG_ERROR("xGateHmpProcessor::handle_maticmsg, EN_SMATIC_MSG_UNKNOWN failed !!!");
						break;

				 }
	 }

	 XGLOG_INFO( "xGateHmpProcessor::handle_grpc_msg End  for msgId:%s,CallId:%s",msg_id.c_str(), call_id.c_str());

	 return EN_XGATE_STATUS_SUCCESS;
}

Value xGateHmpProcessor::setJsonParam(string strValue,Document &doc ){
	 Value retVal;
	 retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
	 return retVal;
}

Value xGateHmpProcessor::setJsonParam(unsigned int iValue, Document &doc){
	 Value retVal;
	 retVal = iValue;
	 return retVal;
}

std::string xGateHmpProcessor::encode_stt_tts_sdp_message(MgMediaDetail mrcp_detail)
{

	 Document doc;
	 StringBuffer strBuf;
	 Writer<StringBuffer> writer(strBuf);
	 Value outputVal(kObjectType);
	 Document::AllocatorType& allocator = doc.GetAllocator();

	 XGLOG_INFO( "xGateHmpProcessor::encode_stt_tts_sdp_message  start for mgresource_id %s : call_id :%s",mrcp_detail.mgresource_id.c_str(),mrcp_detail.call_id.c_str());

	 outputVal.AddMember("mg_resource_id", setJsonParam(mrcp_detail.mgresource_id, doc), allocator);
	 outputVal.AddMember("call_id", setJsonParam(mrcp_detail.call_id, doc), allocator);
	 outputVal.AddMember("codec_name",setJsonParam(mrcp_detail.audioDetail.m_codecName, doc), allocator);
	 outputVal.AddMember("codec", setJsonParam(mrcp_detail.audioDetail.m_codec,doc), allocator);
	 outputVal.AddMember("ptime", setJsonParam(mrcp_detail.audioDetail.m_ptime, doc), allocator);
	 outputVal.AddMember("mrcp_codec_name",setJsonParam(mrcp_detail.mrcp_codec_name, doc), allocator);
	 outputVal.AddMember("mrcp_codec", setJsonParam(mrcp_detail.mrcp_codec,doc), allocator);
	 outputVal.AddMember("mrcp_ptime", setJsonParam(mrcp_detail.mrcp_ptime, doc), allocator);
	 outputVal.AddMember("mrcp_mg_ip", setJsonParam(mrcp_detail.audioDetail.m_serverIp, doc), allocator);
	 outputVal.AddMember("mrcp_mg_port", setJsonParam(to_string(mrcp_detail.audioDetail.m_serverPort), doc), allocator);
	 outputVal.AddMember("mrcp_mg_port", setJsonParam(to_string(mrcp_detail.audioDetail.m_server64Port), doc), allocator);
	 outputVal.AddMember("mrcp_mg_ttsport", setJsonParam(to_string(mrcp_detail.audioDetail.m_serverTtsPort), doc), allocator);
	 outputVal.AddMember("mrcp_server_ip", setJsonParam(mrcp_detail.mrcp_server_ip, doc), allocator);
	 outputVal.AddMember("mrcp_server_port", setJsonParam(mrcp_detail.mrcp_server_port, doc), allocator);
	 outputVal.AddMember("mrcpUid", setJsonParam(mrcp_detail.mrcpUid,doc), allocator);
	 outputVal.AddMember("mrcpCallid", setJsonParam(mrcp_detail.mrcpCallid, doc), allocator);
	 outputVal.AddMember("mrcpDomainid", setJsonParam(mrcp_detail.mrcpDomainid, doc), allocator);
	 outputVal.AddMember("mrcp_Devicetype", setJsonParam(mrcp_detail.mrcp_Devicetype, doc), allocator);
	 outputVal.AddMember("mrcp_Ext", setJsonParam(mrcp_detail.mrcp_Ext, doc), allocator);
	 outputVal.AddMember("mrcp_isSttEnabled", setJsonParam(mrcp_detail.mrcp_isSttEnabled, doc), allocator);
	 outputVal.AddMember("transcript_mode",setJsonParam(mrcp_detail.transcript_mode,doc),allocator);
	 outputVal.AddMember("tts_enabled",setJsonParam(mrcp_detail.tts_enabled,doc),allocator);
	 outputVal.AddMember("is_ccaas",setJsonParam(mrcp_detail.is_ccaas,doc),allocator);
	 outputVal.AddMember("ccaas_port_type",setJsonParam(mrcp_detail.ccaas_port_type,doc),allocator);
	 outputVal.AddMember("ccaas_user_agent",setJsonParam(mrcp_detail.ccaas_user_agent,doc),allocator);
	 outputVal.AddMember("tts_fd",setJsonParam(mrcp_detail.tts_fd,doc),allocator);
	 outputVal.AddMember("ccaas_agent_name",setJsonParam(mrcp_detail.ccaas_agent_name,doc),allocator);
	 outputVal.AddMember("ccaas_agent_email",setJsonParam(mrcp_detail.ccaas_agent_email,doc),allocator);
	 outputVal.AddMember("ccaas_user_id",setJsonParam(mrcp_detail.ccaas_agent_user_id,doc),allocator);
	 outputVal.AddMember("ccaas_cust_firstname",setJsonParam(mrcp_detail.ccaas_cust_firstname,doc),allocator);
	 outputVal.AddMember("ccaas_cust_lastname",setJsonParam(mrcp_detail.ccaas_cust_lastname,doc),allocator);
	 outputVal.AddMember("ccaas_auto_attendant",setJsonParam(mrcp_detail.ccaas_auto_attendant,doc),allocator);
	 outputVal.AddMember("ccaas_virtual_agent",setJsonParam(mrcp_detail.ccaas_virtual_agent,doc),allocator);
	 outputVal.AddMember("ccaas_cli",setJsonParam(mrcp_detail.ccaas_cli,doc),allocator);
	 outputVal.AddMember("ccaas_ddi",setJsonParam(mrcp_detail.ccaas_ddi,doc),allocator);
	 outputVal.AddMember("is_smart_assist",setJsonParam(mrcp_detail.is_smart_assist,doc),allocator);

	 outputVal.Accept(writer);
	 string msg("");
	 msg = strBuf.GetString();

	 XGLOG_INFO( "xGateHmpProcessor::encode_stt_tts_sdp_message  End for mgresource_id %s : call_id :%s",mrcp_detail.mgresource_id.c_str(),mrcp_detail.call_id.c_str());
	 return msg;
}

xGateRetVal xGateHmpProcessor::handle_pipeline_start_request(string call_id,string msg_id,STT_TTS_STATE mrcpState,xGateMrcpCommand mrcpCommand)
{
	 xGateHmpGstManager *pHmpGstManager = xGateUtil::getHmpGstManager();
	 if(!pHmpGstManager) {
			XGLOG_ERROR( "handle_port_allocate failed. Invalid HmpGstManager object !!");
			return EN_XGATE_STATUS_ERROR;
	 }
	 pHmpGstManager->start_channel(call_id,msg_id,mrcpState,mrcpCommand);
	 return EN_XGATE_STATUS_ERROR;
}

gboolean xGateHmpProcessor::handle_pipeline_stop_request(string call_id,string msg_id,STT_TTS_STATE mrcpState,xGateMrcpCommand mrcpCommand)
{
	 gboolean ret = FALSE;
	 xGateHmpGstManager *pHmpGstManager = xGateUtil::getHmpGstManager();
	 if(!pHmpGstManager) {
			XGLOG_ERROR( "handle_port_allocate failed. Invalid HmpGstManager object !!");
			return ret;
	 }
	 ret = pHmpGstManager->stop_channel(call_id,msg_id,mrcpState,mrcpCommand);
	 XGLOG_INFO(" xGateHmpProcessor::handle_mrcp_recongnition_stop_request,capture started !!!");
	 return ret;
}

gboolean xGateHmpProcessor::handle_base64_encoding_start_request(string call_id,string msg_id)
{
	 gboolean ret = FALSE;
	 xGateHmpGstManager *pHmpGstManager = xGateUtil::getHmpGstManager();
	 if(!pHmpGstManager) {
			XGLOG_ERROR( "handle_port_allocate failed. Invalid HmpGstManager object !!");
			return ret;
	 }
	 ret = pHmpGstManager->start_base64_encoding_capture_channel(call_id,msg_id);
	 XGLOG_INFO(" xGateHmpProcessor::handle_base64_encoding_start_request,capture started !!!");
	 return ret;
}

gboolean xGateHmpProcessor::handle_base64_encoding_stop_request(string call_id,string msg_id)
{
	 gboolean ret = FALSE;
	 xGateHmpGstManager *pHmpGstManager = xGateUtil::getHmpGstManager();
	 if(!pHmpGstManager) {
			XGLOG_ERROR( "handle_port_allocate failed. Invalid HmpGstManager object !!");
			return ret;
	 }
	 ret = pHmpGstManager->stop_base64_encoding_capture_channel(call_id,msg_id);
	 XGLOG_INFO(" xGateHmpProcessor::handle_base64_encoding_stop_request,capture started !!!");
	 return ret;
}

bool xGateHmpProcessor::PostMessageToMrcpClient(xGateDbConfigMgrServiceMsg *Servicemsg)
{
	 XGLOG_ERROR("xGateHmpProcessor::PostMessageToMrpcClient,start !!!");
	 switch(Servicemsg->get_mrcp_command())
	 {
			case EN_MSG_PORT_ALLOCATE_REQ:
			case EN_STT_START_REQ:
			case EN_STT_PAUSE_REQ:
			case EN_STT_RESUME_REQ:
			case EN_STT_STOP_REQ:
			case EN_STT_TTS_TRANSFER_ADD_PARTICIPANT:
			case EN_STT_REMOVE_PARTICIPANT:
			case EN_MSG_RELEASE:
			case EN_MRCP_MSG_ALLOCATE_STT_TTS:
			case EN_MRCP_MSG_SYNTHESIZE_STOP:
			case EN_GET_VARAIBLE_STT:
			case EN_MSG_DTMF_DIGITS:
			case EN_TTS_MSG_START_REQ:
			case EN_MRCP_MSG_SYNTHESIZE_REQ:
			case EN_MRCP_MSG_RECOGNITION_STOP:
			case EN_TTS_SPEAK_COMPLETE:
			case EN_POST_BASE64_MSG:
			case EN_VOICE_BOT_DEINIT:
			case EN_STT_START:
				 {
						if(!Servicemsg)
						{
							 XGLOG_ERROR("xGateHmpProcessor::PostMessageToMrcpClient, service msg empty !!!");
							 return false;
						}
						Servicemsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_BRIDGE);
						UR_MRCP_MODULE_BRIDGE::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(Servicemsg));
						break;
				 }

			case EN_MSG_UNKNOWN:
			default:
				 {
						XGLOG_ERROR("xGateHmpProcessor::PostMessageToMrcpClient,EN_MSG_UNKNOWN !!!");
						break;
				 }
	 }
	 return TRUE;
}

xGateRetVal xGateHmpProcessor::post_media_detail(xGateMgMsg *pMgMsg)
{
	 XGLOG_INFO( "Enter xGateHmpProcessor::post_media_detail");
	 xGateMgMsgType msgType = pMgMsg->get_mg_msg_type();
	 xGateMgMsg* pMgMsg1 = NULL;
	 xGateNetConnection netConInfo;
	 //MgMediaDetail &mg_mediaDetail = pMgMsg->get_media_detail();
	 pMgMsg1 = new xGateMgMsg();

	 pMgMsg1->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
	 //printf("xGateHmpProcessor::post_media_detail (posting msg: %d)", pMgMsg->get_mg_msg_type());
	 pMgMsg1->set_mg_msg_type(pMgMsg->get_mg_msg_type());
	 pMgMsg1->set_media_detail(pMgMsg->get_media_detail());
	 //pMgMsg1->set_media_detail(mg_mediaDetail);
	 pMgMsg->get_net_con_info(netConInfo);
	 pMgMsg1->set_net_con_info(netConInfo);
	 MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();

	 cout<<"xGateHmpProcessor::post_media_detail"<<mediaDetail.ccaas_voice_recog_port<<endl;
	 cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;

	 ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
	 taskPtr = xGateUtil::getMgDispatcher();
	 if(!taskPtr) {
			XGLOG_ERROR( "xGateHmpProcessor::post_media_detail MgDispatcher task not found");
			return EN_XGATE_STATUS_ERROR;
	 }

	 ACE_Message_Block* pAmb = NULL;
	 pAmb = static_cast<xGateMgMsg*> (pMgMsg1);
	 ACE_Time_Value tval ((time(NULL) ));
	 tval += ACE_Time_Value(0,1);

	 if (taskPtr->putq(pAmb, &tval)) {
			XGLOG_INFO( "xGateHmpProcessor::post_media_detail MgDispatcher enqueue message success");
	 } else {
			XGLOG_ERROR( "xGateHmpProcessor::post_media_detail MgDispatcher enqueue message failed");
			delete pMgMsg;
			return EN_XGATE_STATUS_ERROR;
	 }

	 return EN_XGATE_STATUS_SUCCESS;
}

#ifdef XGATE_HAS_SELF_AUDIT
xGateRetVal xGateHmpProcessor::handle_audit_msg(xGateAuditMsg* pMsg)
{
	 XGLOG_INFO( "xGateHmpProcessor::handle_audit_msg start");
	 xGateRetVal retVal = EN_XGATE_STATUS_ERROR;

	 if(!pMsg) {
			XGLOG_ERROR( "xGateHmpProcessor::handle_audit_msg failed. xGateAuditMsg pointer is NULL !");
			return EN_XGATE_STATUS_ERROR;
	 }

	 xGateHmpGstManager *pHmpGstManager = xGateUtil::getHmpGstManager();
	 if(!pHmpGstManager) {
			XGLOG_ERROR( "xGateHmpProcessor::handle_audit_msg failed. Invalid HmpGstManager object !");
			return EN_XGATE_STATUS_ERROR;
	 }

	 switch(pMsg->getAuditMsgType()) {
			case EN_AUDIT_MSG_REGISTER_CALL:
				 {
						XGLOG_INFO( "xGateHmpProcessor::handle_audit_msg EN_AUDIT_MSG_REGISTER_CALL");
						break;
				 }
			case EN_AUDIT_MSG_DEREGSITER_CALL:
				 {
						XGLOG_INFO( "xGateHmpProcessor::handle_audit_msg EN_AUDIT_MSG_DEREGSITER_CALL");
						break;
				 }
			case EN_AUDIT_MSG_CHECK_CALL_STATUS:
				 {
						XGLOG_INFO( "xGateHmpProcessor::handle_audit_msg EN_AUDIT_MSG_CHECK_CALL_STATUS");
						break;
				 }
			case EN_AUDIT_MSG_CHECK_CLIENT_STATUS:
				 {
						XGLOG_INFO( "xGateHmpProcessor::handle_audit_msg EN_AUDIT_MSG_CHECK_CLIENT_STATUS");
						return pHmpGstManager->check_client_status(pMsg);
				 }
			case EN_AUDIT_MSG_CLEANUP_CALL:
				 {
						XGLOG_INFO( "xGateHmpProcessor::handle_audit_msg EN_AUDIT_MSG_CLEANUP_CALL");
						break;
				 }
			case EN_AUDIT_MSG_CLEANUP_CLIENT:
				 {
						XGLOG_INFO( "xGateHmpProcessor::handle_audit_msg EN_AUDIT_MSG_CLEANUP_CLIENT");
						retVal = pHmpGstManager->cleanup_client(pMsg);
						break;
				 }
			case EN_AUDIT_MSG_UNKNOWN:
			default:
				 {
						XGLOG_ERROR( "xGateHmpProcessor::handle_audit_msg failed. got EN_AUDIT_MSG_UNKNOWN !");
						break;
				 }      
	 }
	 return EN_XGATE_STATUS_SUCCESS; 
}
#endif
