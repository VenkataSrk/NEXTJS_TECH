#include "xgconfig.h"
#ifdef XGATE_HAS_GSTREAMER 



//local include
#include "xglog.h"

//self include
#include "xGateHmpGstManager.h"
#include "xGateUtil.h"
#define THISMODULE "GstManag"

#define THREE_SEC_SILENCE_IVR "/root/wav_files/sounds/vectone/3sec_silence.wav"
#define FOUR_SEC_SILENCE_IVR "/root/wav_files/sounds/vectone/4sec_silence.wav"
#define FIVE_SEC_SILENCE_IVR "/root/wav_files/sounds/vectone/5sec_silence.wav"
/*
 * Fix for deadlock situation.
 * */
/**
 * Default Constructor
 */
//TODO: time configuration must be dynamic
xGateHmpGstManager::xGateHmpGstManager(const char* serverType)
{
	 XGLOG_INFO( "xGateHmpGstManager::xGateHmpGstManager(%s)",(char*)ur_log_string(serverType));
	 memset(m_serverType,0,SERVER_TYPE_SIZE);
	 strcpy(m_serverType,serverType);
}

/**
 * Destructor
 */
xGateHmpGstManager::~xGateHmpGstManager (void)
{
	 XGLOG_INFO( "xGateHmpGstManager::~xGateHmpGstManager(%s) destructor called",(char*)ur_log_string(m_serverType));
	 m_running = false;
}//end destructor

/**
 * Initialize the xGateHmpGstManager
 */
xGateRetVal xGateHmpGstManager::init (void)
{

	 XGLOG_INFO( "xGateHmpGstManager::init(%s)", (char*)ur_log_string (m_serverType));

	 // Spawn the thread
	 if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
						(void*) this,
						THR_NEW_LWP|THR_DETACHED) == -1)
	 {
			// Spawn Failure
			XGLOG_INFO( "xGateHmpGstManager::init(%s)"\
						" ace error while spawning thread",(char*)ur_log_string(m_serverType));
			// cleanup the timer manager singelton instance
			return EN_XGATE_STATUS_ACE_ERROR;
	 } 
	 return EN_XGATE_STATUS_SUCCESS;  
}//end init

/**
 *Start the event processing loop
 */
void xGateHmpGstManager::run (void* threadClass)
{
	 XGLOG_INFO("xGateHmpGstManager::run (starting gstreamer events processing loop)");

	 xGateHmpGstManager* pGstManager = static_cast<xGateHmpGstManager*>(threadClass);     
	 pGstManager->m_running = true; 

	 //Note: below concepts needs to be build before loop
	 //1. initialize gstreamer library
	 if(pGstManager->initGstreamerLib() != EN_XGATE_STATUS_SUCCESS) {
			XGLOG_ERROR( "xGateHmpGstManager::initGstreamerLib failed xGateHmpGstManager thread exit");
			return; 
	 }

	 //2. initialize Gstreamer MainEventLoop from this thread context
	 GMainContext *ctx = g_main_context_ref_thread_default();
	 GMainLoop *loop = g_main_loop_new (ctx, FALSE);
	 //    all the gstreamer related events handled from this thread context. 
	 while (pGstManager->m_running)
	 {
			//Note: below concepts needs to be build within loop
			g_main_context_iteration (ctx, FALSE); //TRUE - blocking
			usleep(20000); //2000 is 2 milliseconds
	 }

	 //deinitializing gstreamer library
	 gst_deinit();
	 XGLOG_INFO( "xGateHmpGstManager::run reactor loop is ended");
}//end run

/**
 * Interface to stop the HmpGstManager thread
 */
xGateRetVal xGateHmpGstManager::stopThread( void )
{
	 XGLOG_INFO("xGateHmpGstManager::stopThread(%s) called",(char*)ur_log_string(m_serverType));
	 m_running = false;
	 XGLOG_INFO( "xGateHmpGstManager::stopThread(%s) Stopped xGateHmpGstManager", (char*)ur_log_string (m_serverType));
	 return EN_XGATE_STATUS_SUCCESS;
} 

xGateRetVal xGateHmpGstManager::initGstreamerLib()
{
	 XGLOG_INFO("xGateHmpGstManager::initGstreamerLib(%s)",(char*)ur_log_string(m_serverType));
	 GError *err;
	 if(!gst_init_check(0, NULL, &err)) {
			if(err) {
				 XGLOG_ERROR( "xGateHmpGstManager::initGstreamerLib failed to initialize GStreamer. Error: %d: %s", \
							 err->code, err->message); 
				 g_error_free(err);
			} else {
				 XGLOG_ERROR( "xGateHmpGstManager::initGstreamerLib failed to initialize GStreamer. Unknown Error");
			}
			return EN_XGATE_STATUS_ERROR;
	 }

	 guint major, minor, micro, nano;
	 gst_version (&major, &minor, &micro, &nano);
	 XGLOG_INFO( "xGateHmpGstManager::initGstreamerLib successfully initialized Gstreamer version %d.%d.%d-%d", \
				 major, minor, micro, nano);

	 return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpGstManager::allocate_stt_tts_channel(MgMediaDetail &mediaDetail, xGateNetConnection &pbxConn)
{
	 xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
	 gboolean isEntryExist = TRUE;
	 ClientDetail clientDetail;
	 MRCPMediaDetail mrcpmediaDetail;
	 xGateHmpCall *pHmpCall = NULL;
	 xGateHmpConference *pHmpConference = NULL;

	 XGLOG_INFO( "xGateHmpGstManager::allocate_stt_tts_channel for mgresource_id: %s, call_type:%d call_id:%s \ 
				 client_ip:%s client_port:%d codec:%d, sig_type:%d, call_dir %d",(char*)ur_log_string(mediaDetail.mgresource_id), \
				 mediaDetail.call_type, (char*)ur_log_string(mediaDetail.call_id), (char*)ur_log_string(mediaDetail.audioDetail.m_clientIp), mediaDetail.audioDetail.m_clientPort, \
				 mediaDetail.audioDetail.m_codec, mediaDetail.sig_type, mediaDetail.call_dir);

	 if(mediaDetail.mgresource_id.empty() || mediaDetail.call_type == EN_XGATE_CALL_TYPE_UNKNOWN || \
				 mediaDetail.call_id.empty() || mediaDetail.audioDetail.m_clientIp.empty() || mediaDetail.audioDetail.m_clientPort <= 0) {
			XGLOG_ERROR( "xGateHmpGstManager::allocate_stt_tts_channel failed. Invalid media details !!");
			return retVal;
	 }

	 if(mediaDetail.call_type == EN_XGATE_CALL_TYPE_SIMPLE || mediaDetail.call_type == EN_XGATE_CALL_TYPE_CCAAS || mediaDetail.call_type == EN_XGATE_CALL_TYPE_TRANSFER){
			//1. Check whether the Call object created and its entry available in Map using call_id
			pHmpCall = find_call_entry(mediaDetail.mgresource_id);
			if(!pHmpCall) { //first time creating call entry for this call_id
				 pHmpCall = new xGateHmpCall(mediaDetail.mgresource_id, mediaDetail.call_type);
				 isEntryExist = FALSE;
				 XGLOG_ERROR( "xGateHmpGstManager::add_participant creating new channel for resourse %s !!", (char*)ur_log_string(mediaDetail.mgresource_id));
			}
	 } else if(mediaDetail.call_type == EN_XGATE_CALL_TYPE_CONFERENCE){
			pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
			if(!pHmpConference) { //first time creating Conference entry for this mgresource_id
				 XGLOG_ERROR( "xGateHmpGstManager::add_participant creating new conference channel for resourse %s !!", (char*)ur_log_string(mediaDetail.mgresource_id));
				 pHmpConference = new xGateHmpConference(mediaDetail.mgresource_id, mediaDetail.call_type);
				 isEntryExist = FALSE;
			}
	 }


	 retVal = set_media_details(pHmpCall, mediaDetail, clientDetail);

	 //fill pbx connection info in clientDetail itself
	 clientDetail.m_pbxIp = pbxConn.recvIp_;  
	 clientDetail.m_pbxPort = pbxConn.recvPort_;
	 if(clientDetail.m_callType == EN_XGATE_CALL_TYPE_SIMPLE || clientDetail.m_callType == EN_XGATE_CALL_TYPE_CCAAS || mediaDetail.call_type == EN_XGATE_CALL_TYPE_TRANSFER){
			retVal = pHmpCall->create_stt_tts_channel(clientDetail);
			if(retVal == EN_XGATE_STATUS_SUCCESS && !isEntryExist) {
				 add_call_entry(mediaDetail.mgresource_id, pHmpCall);
			}
	 }

	 if(pHmpConference){
			if(mediaDetail.call_type == EN_XGATE_CALL_TYPE_CONFERENCE){
				 retVal = pHmpConference->create_conference_channel(clientDetail);
				 if(retVal == EN_XGATE_STATUS_SUCCESS && !isEntryExist) {
						insert_conference_entry(mediaDetail.mgresource_id,pHmpConference);
				 }
			}
	 }

	 update_media_details(clientDetail, mediaDetail);

#ifdef XGATE_HAS_SELF_AUDIT
	 xGateAuditMsg *auditMsg = new xGateAuditMsg(mediaDetail.call_id, mediaDetail.mgresource_id);
	 auditMsg->setAuditMsgType(EN_AUDIT_MSG_REGISTER_CALL);
	 auditMsg->setAuditState(EN_AUDIT_STATE_CALL_JUST_REGISTERED);
	 AUDITUTIL->postToAudit(auditMsg); 
#endif

	 return retVal;
}

//NOTE: Supporting AUDIO CC for Transfer Call in UCAAS
//Call transfer event will be triggered from PBX after getting 200OK from target party
xGateRetVal xGateHmpGstManager::handle_call_transfer(MgMediaDetail &mediaDetail)
{
	 xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
	 string targetChannelId = mediaDetail.mgresource_id;
	 string transferChannelId = mediaDetail.call_id; //for call transfer call_id is consider to be an another mgresource_id

	 XGLOG_INFO( "handle_call_transfer event for target channel:%s and transfer channel: %s start", \
				 targetChannelId.c_str(), transferChannelId.c_str());

	 if(targetChannelId == transferChannelId) {
			XGLOG_ERROR( "handle_call_transfer failed. target channel_id and transfer channel_id should not be same !");
			return EN_XGATE_STATUS_ERROR;
	 }

	 xGateHmpCall *targetHmpCall, *transferHmpCall = NULL;
	 targetHmpCall = find_call_entry(targetChannelId);
	 if(!targetHmpCall || targetHmpCall->m_clientCount != 1) {
			XGLOG_ERROR( "handle_call_transfer failed. target 'HmpCall' entry not found or client count not equal to one in target channel: %s !", \
						(char*)ur_log_string(targetChannelId));
			return EN_XGATE_STATUS_ERROR;
	 }

	 transferHmpCall = find_call_entry(transferChannelId);
	 if(transferHmpCall)
			XGLOG_INFO("xGateHmpGstManager::handle_call_transfer,targetHmpCall size:%d,transferHmpCall:%d",targetHmpCall->m_clientCount,transferHmpCall->m_clientCount);
	 if(!transferHmpCall || transferHmpCall->m_clientCount != 1) {
			XGLOG_ERROR( "handle_call_transfer failed. transfer 'HmpCall' entry not found or client count not equal to one in transfer channel: %s !!", \
						(char*)ur_log_string(transferChannelId));
			return EN_XGATE_STATUS_ERROR;
	 }

	 Client *targetClient, *transferClient = NULL;
	 targetClient = targetHmpCall->get_client_entry(0);
	 if(!targetClient) {
			XGLOG_ERROR( "handle_call_transfer failed. no 'client' found in transfer channel: %s !", \
						(char*)ur_log_string(targetChannelId));
			return EN_XGATE_STATUS_ERROR;
	 }

	 transferClient = transferHmpCall->get_client_entry(0);
	 if(!transferClient) {
			XGLOG_ERROR("handle_call_transfer failed. no 'client' found in transfer channel: %s !", \
						(char*)ur_log_string(transferChannelId));
			return EN_XGATE_STATUS_ERROR;
	 }

	 //set/update the media details that we got from target(i.e C) party in 200 OK response
	 ClientDetail &targetClientDetail = targetClient->m_detail;

	 //For Call Park call dir is in and scenario like call park no need to set media details again
	 //if(mediaDetail.media_event != EN_MEDIA_EVENT_CALL_PARK)
	 {
			retVal = set_media_details(targetHmpCall, mediaDetail, targetClientDetail);
	 }

	 //make sure the target channel id and callid are same
	 if (mediaDetail.media_event != EN_MEDIA_EVENT_ATTEND_TRANSFER)
	 {
			XGLOG_INFO("xGateHmpGstManager::handle_call_transfer  mediaDetail.media_event=(%d)",mediaDetail.media_event);

			targetClientDetail.m_callId = targetClientDetail.m_mgresourceId;
	 }

	 ClientDetail &transferClientDetail = transferClient->m_detail;

	 //make sure the transfered party channel id has been updated
	 transferClientDetail.m_mgresourceId = targetClientDetail.m_mgresourceId;


	 if(mediaDetail.asr_type == EN_XGATE_ASR_SPEECH_MATICS)
			transferClient->speechMaticsClient->m_uid = transferClientDetail.m_mgresourceId;

	 if(mediaDetail.asr_type == EN_XGATE_ASR_INHOUSE)
			transferClient->inHouseClient->m_uid = transferClientDetail.m_mgresourceId;

   if(mediaDetail.media_event == EN_MEDIA_EVENT_TRANSFER){
	 if(!targetClient->create_stt_tts_audio_channel(targetClient->m_detail.m_audioPort,targetClient->m_audioSocket,0,NULL,0,targetClient)) {
			XGLOG_ERROR( "handle_call_transfer failed. Not able to create audio channel for target client: %s !", \
						(char*)ur_log_string(targetChannelId));
			return EN_XGATE_STATUS_ERROR;
	 }
   }

	 //carefully remove transferClient from existing channel. Note: don't delete client object
	 transferHmpCall->remove_client_fromlist(transferClient);

	 //add transferClient object into target channel
	 targetHmpCall->insert_client_entry(transferClient);

	 erase_call_entry(transferChannelId);
	 delete transferHmpCall;
	 transferHmpCall = NULL;

	 return retVal;
}

//NOTE: Supporting AUDIO CC for Reconnection scenario in UCAAS
xGateRetVal xGateHmpGstManager::reallocate_stt_tts_channel(MgMediaDetail &mediaDetail, xGateNetConnection &pbxConn)
{
	 xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
	 ClientDetail clientDetail;
	 xGateHmpConference *pHmpConference = NULL;
	 xGateHmpCall *newHmpCall, *oldHmpCall = NULL;
	 GstState currState,oldState;
	 Client *newClient=NULL, *oldClient=NULL ,*anotherClient= NULL;
	 gboolean isEntryExist = TRUE;
	 gboolean isJoined = TRUE;

	 XGLOG_INFO( "xGateHmpGstManager::reallocate_stt_tts_channel for mgresource_id: %s, call_type:%d call_id:%s \
				 client_ip:%s client_port:%d codec:%d, sig_type:%d, call_dir %d",(char*)ur_log_string(mediaDetail.mgresource_id), \
				 mediaDetail.call_type, (char*)ur_log_string(mediaDetail.call_id), (char*)ur_log_string(mediaDetail.audioDetail.m_clientIp),\
				 mediaDetail.audioDetail.m_clientPort, mediaDetail.audioDetail.m_codec, mediaDetail.sig_type, mediaDetail.call_dir);

	 if(mediaDetail.mgresource_id.empty() || mediaDetail.call_type == EN_XGATE_CALL_TYPE_UNKNOWN || \
				 mediaDetail.call_id.empty() || mediaDetail.audioDetail.m_clientIp.empty() || mediaDetail.audioDetail.m_clientPort <= 0) {
			XGLOG_ERROR( "xGateHmpGstManager::reallocate_stt_tts_channel failed. Invalid media details !!");
			return retVal;
	 }

	 if(mediaDetail.old_mgresource_id.empty() || mediaDetail.old_call_id.empty()) {
			XGLOG_ERROR( "xGateHmpGstManager::reallocate_stt_tts_channel failed. old call_id or old_mgresourceid is empty !!");
			return retVal;
	 }
	 if(mediaDetail.call_type == EN_XGATE_CALL_TYPE_SIMPLE){
			oldHmpCall = find_call_entry(mediaDetail.mgresource_id);
			if(!oldHmpCall){
				 isEntryExist = FALSE;
				 newHmpCall = new xGateHmpCall(mediaDetail.mgresource_id, mediaDetail.call_type);
				 XGLOG_INFO( "xGateHmpGstManager::reallocate_stt_tts_channel creating new hmpcall for resourceId");
				 oldHmpCall = find_call_entry(mediaDetail.old_mgresource_id);
				 if(!oldHmpCall){
						XGLOG_ERROR( "xGateHmpGstManager::reallocate_stt_tts_channel failed oldhmpcall for resourceId not found");
						return retVal;
				 }
			}else{
				 XGLOG_INFO( "xGateHmpGstManager::reallocate_stt_tts_channel for hmpcall resourceId is found in HmpCall Entry");
				 isEntryExist = TRUE;
			}
			oldClient = oldHmpCall->get_client_entry(mediaDetail.old_call_id);

			if(oldHmpCall->m_clientList.size() > 1){
				 anotherClient=(oldHmpCall->m_clientList[0] == oldClient) ? oldHmpCall->m_clientList[1]: oldHmpCall->m_clientList[0];
				 anotherClient->m_detail.m_mgresourceId = mediaDetail.mgresource_id;
			}
	 }else if (mediaDetail.call_type == EN_XGATE_CALL_TYPE_CONFERENCE){
			oldHmpCall = find_call_entry(mediaDetail.mgresource_id);
			if(!oldHmpCall){
				 pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
				 if(!pHmpConference) {
						XGLOG_ERROR( "reallocate_stt_tts_channnel failed. pHmpConference object not found for mgresource_id:%s",(char*)ur_log_string(mediaDetail.mgresource_id));
						return retVal;
				 }
				 XGLOG_INFO( "xGateHmpGstManager::reallocate_stt_tts_channel for conference resourceId is found in HmpConference Entry");
				 oldClient = pHmpConference->get_participant_entry(mediaDetail.old_call_id);
			}else{
				 XGLOG_INFO( "xGateHmpGstManager::reallocate_stt_tts_channel for conference resourceId is found in HmpCall Entry");
				 isJoined = FALSE;
				 oldClient = oldHmpCall->get_client_entry(mediaDetail.old_call_id);
			}
	 }else {
			XGLOG_ERROR( "reallocate_stt_tts_channnel failed. Invalid call Type:%d for resoureId:%s",(char*)ur_log_string(mediaDetail.mgresource_id));
	 }


	 retVal = set_media_details(oldHmpCall, mediaDetail, clientDetail);

	 //fill pbx connection info in clientDetail itself
	 clientDetail.m_pbxIp = pbxConn.recvIp_;
	 clientDetail.m_pbxPort = pbxConn.recvPort_;

	 if(!oldClient){
			XGLOG_ERROR("reallocate_stt_tts_channel failed.oldClient object not found for callId:%s, calltype %d",\
						(char*)ur_log_string(mediaDetail.old_call_id),mediaDetail.call_type);
			return EN_XGATE_STATUS_ERROR;
	 }
	 newClient = new Client(clientDetail);
   newClient->is_ccaas = clientDetail.m_is_ccaas;
   newClient->ccaas_port_type = clientDetail.m_ccaas_port_type;

	 //retVal = set_client_details(oldClient,newClient);
	 gint audioPort = oldClient->m_audioPort;
	 //gint audioPort = 10006;

	 GSocket *audioSocket = NULL;

	 audioSocket = xGateHmpGstUtil::create_udpsocket(audioPort,G_SOCKET_FAMILY_IPV4);
	 clientDetail.audioDetail.m_serverIp=xGateUtil::getLocalIpAddr(AF_INET);

	 if(audioPort <= 0 || !audioSocket) {
			XGLOG_ERROR( "handle_reallocate_stt_tts_channel failed. Allocated voice port or socket is invalid" );
			return EN_XGATE_STATUS_ERROR;
	 }
	 guint ssrc = 0;


	 if(!newClient->create_stt_tts_audio_channel(audioPort,audioSocket,0,NULL,0,newClient)) {
			XGLOG_ERROR( "xGateHmpCall::create_audio_channels failed in reallocate channel!!");
			return EN_XGATE_STATUS_ERROR;
	 }
   
   if(newClient)
	 newClient->m_pHmpVoiceTransceiverChannel->start();

	 clientDetail.audioDetail.m_serverPort=newClient->m_detail.m_audioPort=audioPort;

	 if(mediaDetail.call_type == EN_XGATE_CALL_TYPE_SIMPLE){
			XGLOG_INFO(THISMODULE,"Deleting Client:%x from simple call\n",oldClient);
			if(oldClient){
				 // gst_element_get_state(oldClient->m_pHmpMRCPVoiceCaptureChannel->m_pipeline,&currState,&oldState,0);
				 oldHmpCall->remove_client_entry(oldClient);
				 oldClient = NULL;
			} else{
				 XGLOG_ERROR("delete_client failed. client object not found for call_id: %s",(char*)ur_log_string(mediaDetail.call_id));
			}
	 }else if (mediaDetail.call_type == EN_XGATE_CALL_TYPE_CONFERENCE){
			if(isJoined == TRUE){
				 XGLOG_INFO(THISMODULE,"Deleting Client:%x from conference call",oldClient);

				 pHmpConference->remove_participant_entry(mediaDetail.old_call_id);
			} else{
				 XGLOG_INFO(THISMODULE,"Deleting Confernce Client:%x from simple call\n",oldClient);
				 oldHmpCall->remove_client_entry(oldClient);
				 oldClient = NULL;
			}    
	 }else {    
			XGLOG_ERROR( "reallocate_stt_tts_channnel failed. Invalid call Type:%d for resoureId:%s",(char*)ur_log_string(mediaDetail.mgresource_id));
	 }
	 if(mediaDetail.call_type == EN_XGATE_CALL_TYPE_SIMPLE){
			if(oldHmpCall->m_clientList.size() == 1){
				 if(!anotherClient){
						XGLOG_ERROR( "handle_reallocate_stt_tts_channel failed. Invalid anotherClient to conenct voice channel" );
						return EN_XGATE_STATUS_ERROR;
				 }
			}
			if(isEntryExist){
				 oldHmpCall->insert_client_entry(newClient);
			}else{
				 newHmpCall->insert_client_entry(newClient);
				 newHmpCall->insert_client_entry(anotherClient);
				 add_call_entry(mediaDetail.mgresource_id,newHmpCall);
				 oldHmpCall->m_clientList.clear();
				 delete oldHmpCall;
				 oldHmpCall = NULL;
				 erase_call_entry(mediaDetail.old_mgresource_id);
			}
	 }else if (mediaDetail.call_type == EN_XGATE_CALL_TYPE_CONFERENCE){
			if(isJoined == TRUE){
				 if(retVal != EN_XGATE_STATUS_SUCCESS){
						XGLOG_ERROR( "xGateHmpCall::create_audio_channels failed!!");
						return retVal;
				 }
			}
	 }else {
			XGLOG_ERROR( "reallocate_stt_tts_channnel failed. Invalid call Type:%d for resoureId:%s",(char*)ur_log_string(mediaDetail.mgresource_id));
	 }

	 update_media_details(clientDetail, mediaDetail);

	 return retVal;
}

xGateRetVal xGateHmpGstManager::media_delete_channel(MgMediaDetail &mediaDetail)
{
	 xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
	 xGateHmpCall *pHmpCall = NULL;
	 Client *client = NULL;
	 xGateHmpConference *pHmpConference = NULL;
	 xGateCallType callType = EN_XGATE_CALL_TYPE_UNKNOWN;

	 XGLOG_INFO("xGateHmpGstManager::media_delete_channel for mgresource_id:%s, call_type:%d call_id:%s \
				 client_ip:%s client_port:%d codec:%d, sig_type:%d, call_dir %d",(char*)ur_log_string(mediaDetail.mgresource_id), \
				 mediaDetail.call_type, (char*)ur_log_string(mediaDetail.call_id), (char*)ur_log_string(mediaDetail.audioDetail.m_clientIp), mediaDetail.audioDetail.m_clientPort, \
				 mediaDetail.audioDetail.m_codec, mediaDetail.sig_type, mediaDetail.call_dir);

	 if(mediaDetail.mgresource_id.empty()) {
			XGLOG_ERROR( "xGateHmpGstManager::media_delete_channel failed. Invalid media details !!");
			return retVal;
	 }

	 callType = mediaDetail.call_type;
	 switch(callType) {
			case EN_XGATE_CALL_TYPE_SIMPLE:
			case EN_XGATE_CALL_TYPE_CCAAS:
				 {
						XGLOG_INFO( "media_delete_channel EN_XGATE_CALL_TYPE_SIMPLE for call_id: %s",(char*)ur_log_string(mediaDetail.call_id));
						//1. Check whether the Call object created and its entry available in Map using mgresource_id
						pHmpCall = find_call_entry(mediaDetail.mgresource_id);
						if(!pHmpCall) {
							 XGLOG_ERROR("media_delete_channel failed. HmpCall object not found for mgresource_id:%s",(char*)ur_log_string(mediaDetail.mgresource_id));
							 return retVal;
						}

						client = pHmpCall->get_client_entry(mediaDetail.call_id);
						XGLOG_INFO("Deleting Client:%x from simple call\n",client);
						if(client) {
							 pHmpCall->remove_client_entry(client);
							 if(pHmpCall->m_clientList.size() <= 0){
									erase_call_entry(mediaDetail.mgresource_id);
									delete pHmpCall;
									pHmpCall = NULL;
							 }
						} else { 
							 XGLOG_ERROR("media_delete_channel failed. client object not found for call_id: %s",(char*)ur_log_string(mediaDetail.call_id)); }
				 }
				 break;
			case EN_XGATE_CALL_TYPE_CONFERENCE:
				 {
						XGLOG_INFO("media_delete_channel EN_XGATE_CALL_TYPE_CONFERENCE for call_id: %s",(char*)ur_log_string(mediaDetail.call_id));
						pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
						if(!pHmpConference) {
							 XGLOG_ERROR( "media_delete_channel failed. pHmpConference object not found for mgresource_id:%s",(char*)ur_log_string(mediaDetail.mgresource_id));
							 return retVal;
						}
						client = pHmpConference->get_participant_entry(mediaDetail.call_id);
						if(!client){
							 XGLOG_ERROR("media_delete_channel failed.client object not found for callId:%s", (char*)ur_log_string(mediaDetail.call_id));
							 return retVal;
						}
						pHmpConference->remove_participant_entry(mediaDetail.call_id);
						if (pHmpConference->m_clientMap.size() <= 0) {
							 delete pHmpConference;
							 pHmpConference = NULL;
							 remove_conference_entry(mediaDetail.mgresource_id);
						}
				 }
				 break;
			case EN_XGATE_CALL_TYPE_UNKNOWN:
			default:
				 {
						XGLOG_ERROR("media_delete_channel failed! unsupported / unknown call type for call_id: %s", (char*)ur_log_string(mediaDetail.call_id));
						return EN_XGATE_STATUS_ERROR; //TODO: as yadav request we disabled this and allowed to send media detail to pbx
				 }
	 }

	 return EN_XGATE_STATUS_SUCCESS;
}


xGateRetVal xGateHmpGstManager::release_stt_tts_channel(MgMediaDetail &mediaDetail)
{
	 xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
	 xGateHmpConference *pHmpConference = NULL;
	 xGateHmpCall *pHmpCall = NULL;
	 Client *client = NULL;

	 XGLOG_INFO( "xGateHmpGstManager::release_stt_tts_channel for mgresource_id:%s, call_type:%d call_id:%s \
				 client_ip:%s client_port:%d codec:%d, sig_type:%d, call_dir %d", (char*)ur_log_string(mediaDetail.mgresource_id), \
				 mediaDetail.call_type,(char*)ur_log_string(mediaDetail.call_id), (char*)ur_log_string(mediaDetail.audioDetail.m_clientIp), mediaDetail.audioDetail.m_clientPort, \
				 mediaDetail.audioDetail.m_codec, mediaDetail.sig_type, mediaDetail.call_dir);

	 if(mediaDetail.mgresource_id.empty()) {
			XGLOG_ERROR( "xGateHmpGstManager::release_stt_tts_channel failed. Invalid media details !!");
			return retVal;
	 }

	 //1. Check whether the Call object created and its entry available in Map using mgresource_id
	 pHmpCall = find_call_entry(mediaDetail.mgresource_id);
	 if(!pHmpCall) {
			if((mediaDetail.call_type == EN_XGATE_CALL_TYPE_CONFERENCE)) {
				 pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
				 if(pHmpConference) {
						if (pHmpConference->m_clientMap.size() <= 0) {
							 delete pHmpConference;
							 pHmpConference = NULL;
							 remove_conference_entry(mediaDetail.mgresource_id);
							 retVal = EN_XGATE_STATUS_SUCCESS;
						}
				 }
			} else {
				 XGLOG_ERROR( "release_stt_tts_channel failed. HmpCall object not found for mgresource_id:%s",(char*)ur_log_string(mediaDetail.mgresource_id));
				 return retVal;
			}
	 } else {
			erase_call_entry(mediaDetail.mgresource_id);
			delete pHmpCall;
			pHmpCall = NULL;
			retVal = EN_XGATE_STATUS_SUCCESS;
	 }
	 return retVal;
}

// NOTE: Starting the transceiver channel for SpeechMatcis and Inhouse for CCAAS Call
// Sending the start recongnition request to the inhouse and speech matics
gboolean xGateHmpGstManager::handle_stt_start_request(string mg_resourceId,string callid)
{
	 gboolean ret = FALSE;
	 xGateHmpCall *pHmpCall = NULL;
	 Client *client =NULL;
	 XGLOG_INFO("xGateHmpGstManager::handle_stt_start_request start for call id : %s",(char*)ur_log_string(callid));
	 pHmpCall = find_call_entry(mg_resourceId);
	 if(!pHmpCall){
			XGLOG_ERROR("xGateHmpGstManager::handle_stt_start_request,unable to find pHmpCall object !!!");
			return FALSE;
	 }
	 client = pHmpCall->get_client_entry(callid);
	 if(!client){
			XGLOG_ERROR("xGateHmpGstManager::handle_stt_start_request,unable to find Client !!!");
			return FALSE;
	 }
	 cout <<"!sending start recog req"<<endl;


	 if(client->speechMaticsClient)
	 {
			client->speechMaticsClient->sendStartRecognitionReq();
			std::cout<<"!recog inprogress "<<client->speechMaticsClient->isRecogProcessing<<std::endl;
	 }
	 else 
			std::cout<<"!smatic client not found"<<std::endl;

	 if(client->inHouseClient)
	 {
			client->inHouseClient->sendStartRecognitionReq();
			std::cout<<"!recog inprogress "<<client->inHouseClient->isRecogProcessing<<std::endl;
	 }
	 else 
			std::cout<<"!inhouse client not found"<<std::endl;

	 cout<<"Starting the Transceiver channel!!!!!"<<endl;
	 ret=client->m_pHmpVoiceTransceiverChannel->start();

	 XGLOG_INFO("xGateHmpGstManager::handle_stt_start_request End for call id : %s",(char*)ur_log_string(callid));

	 return true;
}

// NOTE: Starting the transceiver channel for SpeechMatcis and Inhouse for UCAAS Call
// Sending the start recongnition request to the inhouse and speech matics
gboolean xGateHmpGstManager::start_stt_capture_channel(MgMediaDetail &mediaDetail)
{
	 gboolean ret = FALSE;
	 xGateHmpCall *pHmpCall = NULL;
	 xGateHmpConference *pHmpConference =NULL;
	 Client *client =NULL;
	 GstState currState,oldState;

	 XGLOG_INFO("xGateHmpGstManager::start_stt_capture_channel start for call id : %s",(char*)ur_log_string(mediaDetail.call_id.c_str()));

	 string mg_resourceId=mediaDetail.mgresource_id;
	 string callid=mediaDetail.call_id;
	 xGateCallType callType=mediaDetail.call_type;

	 if(callType == EN_XGATE_CALL_TYPE_SIMPLE || callType == EN_XGATE_CALL_TYPE_TRANSFER){
			pHmpCall = find_call_entry(mg_resourceId);
			if(!pHmpCall){
				 XGLOG_ERROR("xGateHmpGstManager::start_stt_capture_channel,unable to find pHmpCall object !!!");
				 return FALSE;
			}
			client = pHmpCall->get_client_entry(callid);
			if(!client){
				 XGLOG_ERROR("xGateHmpGstManager::start_stt_capture_channel,unable to find Client !!!");
				 return FALSE;
			}
	 }
	 else if(callType == EN_XGATE_CALL_TYPE_CONFERENCE){
			pHmpConference = find_conference_entry(mg_resourceId);
			if(!pHmpConference){
				 XGLOG_ERROR("xGateHmpGstManager::start_stt_capture_channel, unbale to find pHmpConference object !!!");
				 return FALSE;
			}
			client =pHmpConference->get_participant_entry(callid);
			if(!client){
				 XGLOG_ERROR("xGateHmpGstManager::start_stt_capture_channel,unable to find Client !!!");
				 return FALSE;
			}
	 }
	 else{
			XGLOG_ERROR("xGateHmpGstManager::start_stt_capture_channel,Invalid callType:%d mgresourceId:%s callId:%s",callType,mg_resourceId.c_str(),callid.c_str());
			return FALSE;
	 }
	 client->m_detail.m_mrcp_isSttEnabled = mediaDetail.mrcp_isSttEnabled;

	 if(client->m_detail.m_asrType == EN_XGATE_ASR_SPEECH_MATICS || client->m_detail.m_asrType == EN_XGATE_ASR_INHOUSE){
			gst_element_get_state(client->m_pHmpVoiceTransceiverChannel->m_pipeline,&currState,&oldState,0);
	 }

	 if(currState != GST_STATE_NULL){
			XGLOG_ERROR("xGateHmpGstManager::start_stt_capture_channel,HmpMRCPVoiceCaptureChannel was played already !!!");
			return ret;
	 }

	 if(client)
			ret = client->start_stt_voice_channel();
	 else
			XGLOG_ERROR("xGateHmpGstManager::start_stt_capture_channel,HmpMRCPVoiceCaptureChannel client not created properly !!!");

	 if(client->m_detail.m_asrType == EN_XGATE_ASR_SPEECH_MATICS)
	 {
			if(client->speechMaticsClient)
			{
				 client->speechMaticsClient->sendStartRecognitionReq();
				 std::cout<<"!recog inprogress "<<client->speechMaticsClient->isRecogProcessing<<std::endl;
			}
			else
			{
				 std::cout<<"!smatic client not found"<<std::endl;
			}
	 }

	 if(client->m_detail.m_asrType == EN_XGATE_ASR_INHOUSE)
	 {
			if(client->inHouseClient)
			{
				 client->inHouseClient->sendStartRecognitionReq();
				 std::cout<<"!recog inprogress "<<client->inHouseClient->isRecogProcessing<<std::endl;
			}
			else
			{
				 std::cout<<"!inhouse client not found"<<std::endl;
			}
	 }

	 XGLOG_INFO("xGateHmpGstManager::start_stt_capture_channel End for call id : %s",(char*)ur_log_string(callid));
	 return ret;
}

// NOTE: Stoping the transceiver channel for SpeechMatcis and Inhouse for UCAAS
// Sending the start recongnition request to the inhouse and speech matics
gboolean xGateHmpGstManager::stop_stt_capture_channel(MgMediaDetail mediaDetail)
{
	 gboolean ret = FALSE;
	 xGateHmpConference *pHmpConference =NULL;
	 Client *client =NULL;
	 GstState currState,oldState;
	 string mg_resourceId=mediaDetail.mgresource_id;
	 string callid=mediaDetail.call_id;
	 xGateCallType callType=mediaDetail.call_type;
	 xGateHmpCall *pHmpCall = NULL;

	 XGLOG_INFO("xGateHmpGstManager::stop_stt_capture_channel start for call id : %s",(char*)ur_log_string(callid));
	 if(callType == EN_XGATE_CALL_TYPE_SIMPLE || callType == EN_XGATE_CALL_TYPE_TRANSFER){
			pHmpCall = find_call_entry(mg_resourceId);
			if(!pHmpCall){
				 XGLOG_ERROR("xGateHmpGstManager::stop_stt_capture_channel,pHmpCall is empty !!!");
				 return ret;
			}
			client =  pHmpCall->get_client_entry(callid);
			if(!client){
				 XGLOG_ERROR("xGateHmpGstManager::stop_stt_capture_channel,client i s empty !!!");
				 return ret;
			}
	 }
	 else if(callType == EN_XGATE_CALL_TYPE_CONFERENCE){
			pHmpConference = find_conference_entry(mg_resourceId);
			if(!pHmpConference){
				 XGLOG_ERROR("xGateHmpGstManager::stop_stt_capture_channel, unbale to find pHmpConference object !!!");
				 return FALSE;
			}
			client =pHmpConference->get_participant_entry(callid);
			if(!client){
				 XGLOG_ERROR("xGateHmpGstManager::stop_stt_capture_channel,unable to find Client !!!");
				 return FALSE;
			}
	 }
	 else{

			return FALSE;
	 }
	 ret = client->stop_stt_voice_channel();

	 if(client->m_detail.m_asrType == EN_XGATE_ASR_SPEECH_MATICS)
	 {
			if(client->speechMaticsClient)
			{
				 client->speechMaticsClient->stop_req();
			}
			else
			{
				 std::cout<<"!smatic client not found"<<std::endl;
			}
	 }

	 if(client->m_detail.m_asrType == EN_XGATE_ASR_INHOUSE)
	 {
			if(client->inHouseClient)
			{
				 client->inHouseClient->stop_req();
			}
			else
			{
				 std::cout<<"!inHouse client not found"<<std::endl;
			}
	 }

	 XGLOG_INFO("xGateHmpGstManager::stop_stt_capture_channel End for call id : %s",(char*)ur_log_string(callid));
	 return ret;
}

// NOTE: Starting the BASE64 pipeline

gboolean xGateHmpGstManager::start_base64_encoding_capture_channel(string callid,string msg_resourceId)
{
	 XGLOG_INFO("xGateHmpGstManager::start_base64_encoding_capture_channel for callId :%s",(char*)ur_log_string(callid));
	 gboolean ret = FALSE; 
	 xGateHmpCall *pHmpCall = NULL;
	 Client *client =NULL;
	 pHmpCall = find_call_entry(msg_resourceId);
	 client =  pHmpCall->get_client_entry(callid);
	 ret = client->start_base64_voice_capture_channel();
	 return ret;
}

gboolean xGateHmpGstManager::stop_base64_encoding_capture_channel(string callid,string msg_resourceId)
{
	 XGLOG_INFO("xGateHmpGstManager::stop_base64_encoding_capture_channel for callId :%s",(char*)ur_log_string(callid));
	 gboolean ret = FALSE;
	 xGateHmpCall *pHmpCall = NULL;
	 Client *client =NULL;
	 pHmpCall = find_call_entry(msg_resourceId);
	 client =  pHmpCall->get_client_entry(callid);
	 ret = client->stop_base64_voice_capture_channel();
	 return ret;
}

// NOTE: Starting the STT and TTS channels for CCAAS
xGateRetVal xGateHmpGstManager::start_channel(string callid,string msg_resourceId,STT_TTS_STATE mrcpState,xGateMrcpCommand mrcpCommand)
{
	 XGLOG_INFO("xGateHmpGstManager::start_channel for callId :%s",(char*)ur_log_string(callid));
	 xGateHmpCall *pHmpCall = NULL;
	 Client *client = NULL;
	 pHmpCall = find_call_entry(msg_resourceId);
	 if(!pHmpCall)
			return EN_XGATE_STATUS_ERROR;
	 client = pHmpCall->get_client_entry(callid);
	 if(!client)
			return EN_XGATE_STATUS_ERROR;

	 client->start_voice_channel(mrcpState,mrcpCommand);

	 return EN_XGATE_STATUS_SUCCESS;
}

// NOTE: Stoping the the STT and TTS channels for CCAAS
gboolean xGateHmpGstManager::stop_channel(string callid,string msg_resourceId,STT_TTS_STATE mrcpState,xGateMrcpCommand mrcpCommand)
{
	 XGLOG_INFO("xGateHmpGstManager::stop_channel for callId :%s",(char*)ur_log_string(callid));
	 gboolean ret = FALSE; 
	 xGateHmpCall *pHmpCall = NULL;
	 Client *client =NULL;
	 pHmpCall = find_call_entry(msg_resourceId);
	 client =  pHmpCall->get_client_entry(callid);
	 ret = client->stop_voice_channel(mrcpState,mrcpCommand);
	 return ret;
}

xGateHmpCall * xGateHmpGstManager::find_call_entry(string callId)
{
	 xGateHmpCall *pHmpCall = NULL;
	 if(m_hmpCallMap.find(callId.c_str(), pHmpCall) != -1) {
			XGLOG_INFO( "find_call_entry success for mgresource_id: %s",(char*)ur_log_string(callId));
	 } else {
			//XGLOG_ERROR( "find_call_entry failed for mgresource_id: %s", callId.c_str());
	 }
	 return pHmpCall;
}

xGateRetVal xGateHmpGstManager::add_call_entry(string callId, xGateHmpCall *pHmpCall)
{
	 xGateRetVal retVal = EN_XGATE_STATUS_ERROR; 
	 int status = -1;
	 status = m_hmpCallMap.bind(callId.c_str(), pHmpCall);
	 if(status == 0) {
			XGLOG_INFO( "add_call_entry success for mgresource_id: %s",(char*)ur_log_string(callId));
			retVal = EN_XGATE_STATUS_SUCCESS;
	 } else if(status == 1) {
			XGLOG_INFO( "add_call_entry attempt is made to bind an existing entry for mgresource_id: %s",(char*)ur_log_string(callId));
	 } else if(status == -1) {
			XGLOG_ERROR( "add_call_entry failed for mgresource_id: %s", (char*)ur_log_string(callId)); 
	 }
	 return retVal;
}

xGateRetVal xGateHmpGstManager::erase_call_entry(string callId)
{
	 XGLOG_INFO( "erase_call_entry_in_map for mgresource_id: %s",(char*)ur_log_string(callId)); 
	 m_hmpCallMap.unbind(callId.c_str());
}

// Start Conference MAP related interfaces 
xGateHmpConference* xGateHmpGstManager::find_conference_entry(string conferenceId)
{
	 xGateHmpConference* pHmpConference = NULL;
	 if(m_hmpConfMap.find(conferenceId.c_str(), pHmpConference) != -1) {
			XGLOG_INFO( "find_conference_entry success for conference_Id: %s",(char*)ur_log_string(conferenceId));
	 } else {
			XGLOG_ERROR( "find_conference_entry failed for conference_Id: %s", (char*)ur_log_string(conferenceId));
	 }
	 return pHmpConference;
} 

xGateRetVal xGateHmpGstManager::insert_conference_entry(string conferenceId, xGateHmpConference* pHmpConference)
{
	 xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
	 int status = -1;

	 status = m_hmpConfMap.bind(conferenceId.c_str(), pHmpConference);
	 if(status == 0) {
			XGLOG_INFO( "insert_conference_entry success for conferenceId: %s", (char*)ur_log_string(conferenceId));
			retVal = EN_XGATE_STATUS_SUCCESS;
	 } else if(status == 1) {
			XGLOG_INFO( "insert_conference_entry attempt is made to bind an existing entry for conferenceId: %s", (char*)ur_log_string(conferenceId));
	 } else if(status == -1) {
			XGLOG_ERROR( "insert_conference_entry failed for conferenceId: %s",(char*)ur_log_string(conferenceId));
	 }
	 return retVal;
}

xGateRetVal xGateHmpGstManager::remove_conference_entry(string conferenceId)
{
	 XGLOG_INFO( "remove_joiners_entry_in_map for conferenceId: %s",(char*)ur_log_string(conferenceId));
	 m_hmpConfMap.unbind(conferenceId.c_str());
}
// End Conference MAP related interfaces



gboolean xGateHmpGstManager::handle_restart_stt_recognition(string mg_resourceId,string callid)
{
	 XGLOG_INFO("xGateHmpGstManager::handle_restart_stt_recognition!!!");
	 gboolean ret = FALSE;
	 xGateHmpCall *pHmpCall = NULL;
	 Client *client =NULL;
	 pHmpCall = find_call_entry(mg_resourceId);
	 if(!pHmpCall){
			XGLOG_ERROR("xGateHmpGstManager::handle_restart_stt_recognition,unable to find pHmpCall object !!!");
			return FALSE;
	 }
	 client = pHmpCall->get_client_entry(callid);
	 if(!client){
			XGLOG_ERROR("xGateHmpGstManager::handle_restart_stt_recognition,unable to find Client !!!");
			return FALSE;
	 }
	 ret = client->CreateInhouseClient();
	 return ret;
}

gboolean xGateHmpGstManager::handle_create_stt_socket_connection(string mg_resourceId,string callid)
{
   XGLOG_INFO("xGateHmpGstManager::handle_create_stt_socket_connection!!!");
   gboolean ret = FALSE;
   xGateHmpCall *pHmpCall = NULL;
   Client *client =NULL;
   pHmpCall = find_call_entry(mg_resourceId);
   if(!pHmpCall){
      XGLOG_ERROR("xGateHmpGstManager::handle_create_stt_socket_connection to find pHmpCall object !!!");
      return FALSE;
   }
   client = pHmpCall->get_client_entry(callid);
   if(!client){
      XGLOG_ERROR("xGateHmpGstManager::handle_create_stt_socket_connection,unable to find Client !!!");
      return FALSE;
   }
   
   if(client->m_detail.m_asrType == EN_XGATE_ASR_SPEECH_MATICS)
     {
       cout<<"The SpeechmaticsClient triggred!!!!"<<endl;
       XGLOG_INFO("xGateHmpCall::create_stt_tts_audio_channels CreateSpeechmaticsClient !!!!" );
       if(!client->CreateSpeechmaticsClient())
        {
          XGLOG_ERROR("xGateHmpGstManager::handle_create_stt_socket_connection CreateSpeechmaticsClient failed" );
          return EN_XGATE_STATUS_ERROR;
        }
        client->speechMaticsClient->m_uid = client->m_detail.m_mgresourceId;
        client->speechMaticsClient->m_callid = client->m_detail.m_callId;
     } else if(client->m_detail.m_asrType == EN_XGATE_ASR_INHOUSE)
     {
        cout<<"The CreateInhouseClient triggred!!!!"<<endl;
        XGLOG_INFO("xGateHmpGstManager::handle_create_stt_socket_connection , The inhouse client triggred!!!!");
        if(!client->CreateInhouseClient())
          {
            XGLOG_ERROR("xGateHmpGstManager::handle_create_stt_socket_connection CreateInhouseClient failed" );
            return EN_XGATE_STATUS_ERROR;
          }
    }
   return ret;
}

gboolean xGateHmpGstManager::handle_stt_recognition_pause_resume(string mg_resourceId,string callid,gboolean recognitionStatus)
{
  gboolean ret = FALSE;
  xGateHmpCall *pHmpCall = NULL;
  Client *client =NULL;
  pHmpCall = find_call_entry(mg_resourceId);
     if(!pHmpCall){
        XGLOG_ERROR("xGateHmpGstManager::handle_stt_recognition_pause_resume,unable to find pHmpCall object !!!");
        return FALSE;
     }
     client = pHmpCall->get_client_entry(callid);
     if(!client){
        XGLOG_ERROR("xGateHmpGstManager::handle_stt_recognition_pause_resume,unable to find Client !!!");
        return FALSE;
     }
     if(client->inHouseClient)
     {
       if(recognitionStatus)
       {
         client->inHouseClient->isRecogProcessing = TRUE;
       }
       else
       {
         client->inHouseClient->isRecogProcessing = FALSE;
       }
     } 
   return true;
}

xGateRetVal xGateHmpGstManager::set_media_details(xGateHmpCall *pHmpCall, MgMediaDetail &mediaDetail, ClientDetail &clientDetail)
{
	 XGLOG_INFO( "xGateHmpGstManager::set_media_details for mgresourceId: %s",(char*)ur_log_string(mediaDetail.mgresource_id));
	 clientDetail.m_mgresourceId = mediaDetail.mgresource_id;
	 clientDetail.m_callType = mediaDetail.call_type;
	 clientDetail.m_asrType = mediaDetail.asr_type;
	 clientDetail.m_ttsType = mediaDetail.tts_type;

	 if (mediaDetail.media_event != EN_MEDIA_EVENT_ATTEND_TRANSFER)
	 {
			XGLOG_INFO("xGateHmpGstManager::set_media_details  mediaDetail.media_event=(%d) call id(%s) ",\
						mediaDetail.media_event,mediaDetail.call_id.c_str());
			clientDetail.m_callId = mediaDetail.call_id;
	 }

	 clientDetail.m_legId = (mediaDetail.leg_id > 0) ? (mediaDetail.leg_id-1) : 0;
	 clientDetail.m_callDir = mediaDetail.call_dir;
	 clientDetail.m_sigType = mediaDetail.sig_type;
	 clientDetail.m_mediaEvent = mediaDetail.media_event;
       
         clientDetail.m_group_callId = mediaDetail.group_call_id;
         clientDetail.m_switch_callId = mediaDetail.switch_call_id;

	 clientDetail.audioDetail.m_mediaType = mediaDetail.audioDetail.m_mediaType;
	 clientDetail.audioDetail.m_clientIp = mediaDetail.audioDetail.m_clientIp;
	 mediaDetail.audioDetail.m_clientIp = "";
	 clientDetail.audioDetail.m_clientPort = mediaDetail.audioDetail.m_clientPort;
	 mediaDetail.audioDetail.m_clientPort = 0;
	 clientDetail.audioDetail.m_codec = mediaDetail.audioDetail.m_codec;
	 clientDetail.audioDetail.m_codecName = mediaDetail.audioDetail.m_codecName;
	 clientDetail.audioDetail.m_ptime = mediaDetail.audioDetail.m_ptime;
	 clientDetail.audioDetail.m_maxPtime = mediaDetail.audioDetail.m_maxPtime;
	 clientDetail.audioDetail.m_samplingRate = mediaDetail.audioDetail.m_samplingRate;
	 clientDetail.audioDetail.m_dtmfSamplingRate = mediaDetail.audioDetail.m_dtmfSamplingRate;

	 clientDetail.audioDetail.m_inBandDtmf = mediaDetail.audioDetail.m_inBandDtmf;
	 clientDetail.audioDetail.m_dtmfpt= mediaDetail.audioDetail.m_dtmfpt;


	 //update the MRCP details

	 clientDetail.m_mrcp_codec_name = mediaDetail.mrcp_codec_name;
	 clientDetail.m_mrcp_codec =mediaDetail.mrcp_codec;
	 clientDetail.m_mrcp_ptime = mediaDetail.mrcp_ptime;
	 clientDetail.m_mrcp_mg_port = mediaDetail.mrcp_mg_port;
	 clientDetail.m_mrcp_mg_ip = mediaDetail.mrcp_mg_ip;
	 clientDetail.m_mrcp_server_port = mediaDetail.mrcp_server_port;
	 clientDetail.m_mrcp_server_ip = mediaDetail.mrcp_server_ip;
	 clientDetail.m_mrcpUid = mediaDetail.mrcpUid;
	 clientDetail.m_mrcpCallid = mediaDetail.mrcpCallid;
	 clientDetail.m_mrcpDomainid = mediaDetail.mrcpDomainid;
	 clientDetail.m_mrcp_Devicetype = mediaDetail.mrcp_Devicetype;
	 clientDetail.m_mrcp_Ext = mediaDetail.mrcp_Ext;
	 clientDetail.m_mrcp_isSttEnabled = mediaDetail.mrcp_isSttEnabled;
	 clientDetail.m_transcript_mode = mediaDetail.transcript_mode;
	 clientDetail.m_mg_Ip = mediaDetail.mg_Ip;
	 clientDetail.m_mg_port = mediaDetail.mg_port;
	 clientDetail.m_is_ccaas = mediaDetail.is_ccaas;
	 clientDetail.m_ccaas_port_type = mediaDetail.ccaas_port_type;



	 //TODO:Yoga, media_proto detail should be a enum not as string
	 if(mediaDetail.media_proto == "RTP") {
			clientDetail.m_mediaModeIn = EN_MEDIA_RTP;
	 } else if(mediaDetail.media_proto == "SRTP") {
			clientDetail.m_mediaModeIn = EN_MEDIA_SRTP;
	 } else if(mediaDetail.media_proto == "DTLS") {
			clientDetail.m_mediaModeIn = EN_MEDIA_DTLS;
	 }

	 //TODO:Yoga, out_proto detail should be a enum not as string
	 if(mediaDetail.out_proto == "RTP") {
			clientDetail.m_mediaModeOut = EN_MEDIA_RTP;
	 } else if(mediaDetail.out_proto == "SRTP") {
			clientDetail.m_mediaModeOut = EN_MEDIA_SRTP;
	 } else if(mediaDetail.out_proto == "DTLS") {
			clientDetail.m_mediaModeOut = EN_MEDIA_DTLS;
	 }

	 return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpGstManager::update_media_details(ClientDetail &clientDetail, MgMediaDetail &mediaDetail)
{
	 XGLOG_INFO("xGateHmpGstManager::update_media_details ");
	 mediaDetail.audioDetail.m_mediaType = clientDetail.audioDetail.m_mediaType;
	 mediaDetail.audioDetail.m_clientIp = clientDetail.audioDetail.m_clientIp;
	 mediaDetail.audioDetail.m_clientPort = clientDetail.audioDetail.m_clientPort;
	 mediaDetail.audioDetail.m_dialIp = mediaDetail.audioDetail.m_respIp = clientDetail.audioDetail.m_serverIp;
	 mediaDetail.audioDetail.m_respPort = clientDetail.audioDetail.m_serverPort;
	 mediaDetail.audioDetail.m_ptime = clientDetail.audioDetail.m_ptime;
	 mediaDetail.audioDetail.m_maxPtime = clientDetail.audioDetail.m_maxPtime;

	 mediaDetail.mrcp_port =clientDetail.m_mrcp_port;
	 mediaDetail.mrcp_ip_info = clientDetail.m_mrcp_ip_info;
	 mediaDetail.tts_fd  = clientDetail.m_tts_fd;


	 mediaDetail.audioDetail.m_serverPort = clientDetail.audioDetail.m_serverPort;
	 mediaDetail.audioDetail.m_serverIp = clientDetail.audioDetail.m_serverIp;
	 mediaDetail.audioDetail.m_serverTtsPort = clientDetail.audioDetail.m_serverTtsPort;
	 mediaDetail.audioDetail.m_server64Port  = clientDetail.audioDetail.m_server64Port;

	 cout<<"The update details are mediaDetail.ccaas_voice_recog_port"<<mediaDetail.audioDetail.m_server64Port<<"clientDetail.m_ccaas_voice_recog_port"<<clientDetail.audioDetail.m_server64Port<<endl;

	 cout<<"The update details "<<mediaDetail.audioDetail.m_serverPort<<endl;
	 cout<<"The update details "<<mediaDetail.audioDetail.m_serverIp<<endl;
	 return EN_XGATE_STATUS_SUCCESS;
}

// NOTE: Posting the Event MSG to the PBX after getting the response from the Bridge Modeule
gboolean xGateHmpGstManager::post_transfer_msg(string mg_resourceId,string callid,string transferNum)
{
	 gboolean ret = FALSE;
	 xGateHmpCall *pHmpCall = NULL;
	 Client *client =NULL;
	 XGLOG_INFO("xGateHmpGstManager::post_transfer_msg starts");
	 pHmpCall = find_call_entry(mg_resourceId);
	 if(!pHmpCall){
			XGLOG_ERROR("xGateHmpGstManager::post_transfer_msg,unable to find pHmpCall object !!!");
			return FALSE;
	 }
	 client = pHmpCall->get_client_entry(callid);
	 if(!client){
			XGLOG_ERROR("xGateHmpGstManager::post_transfer_msg,unable to find Client !!!");
			return FALSE;
	 }
	 client->m_detail.m_transExt=transferNum;
	 client->post_media_event_result(EN_MEDIA_EVENT_TRANSFER);
}

gboolean xGateHmpGstManager::post_tts_complete_msg(string mg_resourceId,string callid)
{
	 gboolean ret = FALSE;
	 xGateHmpCall *pHmpCall = NULL;
	 Client *client =NULL;
	 XGLOG_INFO("xGateHmpGstManager::post_tts_complete_msg starts");
	 pHmpCall = find_call_entry(mg_resourceId);
	 if(!pHmpCall){
			XGLOG_ERROR("xGateHmpGstManager::post_transfer_msg,unable to find pHmpCall object !!!");
			return FALSE;
	 }
	 client = pHmpCall->get_client_entry(callid);
	 if(!client){
			XGLOG_ERROR("xGateHmpGstManager::post_transfer_msg,unable to find Client !!!");
			return FALSE;
	 }
	 client->post_media_event_result(EN_MEDIA_EVENT_PLAY_TEXT_EOS);
}

gboolean xGateHmpGstManager::post_stt_tts_alloc_res_msg(string mg_resourceId,string callid)
{
	 gboolean ret = FALSE;
	 xGateHmpCall *pHmpCall = NULL;
	 Client *client =NULL;
	 xGateHmpConference *pHmpConference = NULL;
	 XGLOG_INFO("xGateHmpGstManager::post_stt_tts_alloc_res_msg starts");

	 pHmpCall = find_call_entry(mg_resourceId);
	 if(pHmpCall){
			client = pHmpCall->get_client_entry(callid);
			if(!client){
				 XGLOG_ERROR("xGateHmpGstManager::post_stt_tts_alloc_res_msg, unable to find Client !!!");
				 return FALSE;
			}
	 }else{
			pHmpConference = find_conference_entry(mg_resourceId);
			if(pHmpConference){
				 client = pHmpConference->get_participant_entry(callid);
				 if(!client){
						XGLOG_ERROR("xGateHmpGstManager::post_stt_tts_alloc_res_msg, unable to find for callId:%s", (char*)ur_log_string(callid));
						return EN_XGATE_STATUS_SUCCESS;
				 }
			} 
	 }

   if(client)
	 client->post_media_event_result(EN_MEDIA_EVENT_STT_TTS_ALLOC_RES);

	 return TRUE;
}

#ifdef XGATE_HAS_SELF_AUDIT
xGateRetVal xGateHmpGstManager::check_client_status(xGateAuditMsg *pMsg)
{
	 string callId(pMsg->getUid());
	 string channelId(pMsg->getChannelId());

	 if(callId.empty() && channelId.empty()) {
			XGLOG_ERROR( "check_client_status failed. callId or channelId is empty !"); 
			return EN_XGATE_STATUS_ERROR;
	 }

	 XGLOG_INFO( "check_client_status for given mgresource_id: %s and client_id: %s", \
				 (char*)ur_log_string(channelId), (char*)ur_log_string(callId));

	 xGateAuditMsg *pAuditMsg = new xGateAuditMsg(callId, channelId);
	 pAuditMsg->setAuditMsgType(EN_AUDIT_MSG_CHECK_CLIENT_STATUS);

	 Client *client = NULL;
	 xGateHmpCall *pHmpCall = NULL;
	 xGateHmpConference *pHmpConference = NULL;

	 //check it in HmpCall entry map
	 pHmpCall = find_call_entry(channelId);
	 if(!pHmpCall) { 
			//check it in HmpConference entry map
			pHmpConference = find_conference_entry(channelId);
			if(!pHmpConference) {
				 XGLOG_INFO( "check_client_status, no call entry found for mgresource_id:%s", \
							 (char*)ur_log_string(channelId));
				 pAuditMsg->setAuditState(EN_AUDIT_STATE_CALL_ENTRY_DELETED);
				 AUDITUTIL->postToAudit(pAuditMsg); 
				 return EN_XGATE_STATUS_SUCCESS;
			}
			client = pHmpConference->get_participant_entry(callId);
	 } else {
			client = pHmpCall->get_client_entry(callId);
	 }

	 if(!client) {
			XGLOG_INFO( "check_client_status not able to identify client entry in channel: %s", \
						(char*)ur_log_string(channelId));
			pAuditMsg->setAuditState(EN_AUDIT_STATE_CLIENT_ENTRY_DELETED);
			AUDITUTIL->postToAudit(pAuditMsg); 
			return EN_XGATE_STATUS_SUCCESS;
	 }

	 //TODO: Yoga, if call entry as well as client entry is available then do deep analyze on channels activity
	 pAuditMsg->setAuditState(EN_AUDIT_STATE_CLIENT_ENTRY_AVAILABLE);
	 AUDITUTIL->postToAudit(pAuditMsg);

	 return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpGstManager::cleanup_client(xGateAuditMsg *pMsg)
{
	 string callId(pMsg->getUid());
	 string channelId(pMsg->getChannelId());

	 if(callId.empty() && channelId.empty()) {
			XGLOG_ERROR( "cleanup_client failed. callId or channelId is empty !"); 
			return EN_XGATE_STATUS_ERROR;
	 }

	 XGLOG_INFO( "cleanup_client for given mgresource_id: %s and client_id: %s", \
				 (char*)ur_log_string(channelId),(char*)ur_log_string(callId));

	 xGateAuditMsg *pAuditMsg = new xGateAuditMsg(callId, channelId);
	 pAuditMsg->setAuditMsgType(EN_AUDIT_MSG_CHECK_CLIENT_STATUS);

	 Client *client = NULL;
	 xGateCallType callType = EN_XGATE_CALL_TYPE_UNKNOWN;
	 xGateHmpCall *pHmpCall = NULL;
	 xGateHmpConference *pHmpConference = NULL;

	 //check it in HmpCall entry map
	 pHmpCall = find_call_entry(channelId);
	 if(!pHmpCall) { 
			//check it in HmpConference entry map
			pHmpConference = find_conference_entry(channelId);
			if(!pHmpConference) {
				 XGLOG_INFO( "cleanup_client, no call entry found for channel_id: %s !", \
							 (char*)ur_log_string(channelId));
				 pAuditMsg->setAuditState(EN_AUDIT_STATE_CALL_ENTRY_DELETED);
				 AUDITUTIL->postToAudit(pAuditMsg); 
				 return EN_XGATE_STATUS_SUCCESS;
			}
			callType = EN_XGATE_CALL_TYPE_CONFERENCE;
			client = pHmpConference->get_participant_entry(callId);
	 } else {
			client = pHmpCall->get_client_entry(callId);
			callType = EN_XGATE_CALL_TYPE_SIMPLE;
	 }

	 if(!client) {
			XGLOG_INFO( "cleanup_client not able to identify client entry in channel_id: %s !", \
						(char*)ur_log_string(channelId));
			pAuditMsg->setAuditState(EN_AUDIT_STATE_CLIENT_ENTRY_DELETED);
			AUDITUTIL->postToAudit(pAuditMsg); 
			return EN_XGATE_STATUS_SUCCESS;
	 }

	 switch(callType) {
			case EN_XGATE_CALL_TYPE_SIMPLE:
				 {
						erase_call_entry(channelId);
						delete pHmpCall;
						pHmpCall = NULL;
						break;
				 } 
			case EN_XGATE_CALL_TYPE_CONFERENCE:
				 {
						if (pHmpConference->m_clientMap.size() <= 0) {
							 delete pHmpConference;
							 pHmpConference = NULL;
							 remove_conference_entry(channelId);
						}
						break;
				 }
			default:
				 {
						XGLOG_ERROR( "cleanup_client failed. Unknown call type for given channel_id: %s !", \
									(char*)ur_log_string(channelId));
						break;
				 }
	 }
	 return EN_XGATE_STATUS_SUCCESS;
}

#endif

#endif
