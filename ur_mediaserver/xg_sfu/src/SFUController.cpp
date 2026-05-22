
#include "xglog.h"
#include "stringencode.h"
#include "xGateSFUtil.h"
#include "xGateHmpGstBin.h"
#define THISMODULE "GstManag"

#define ROOT_TMP_DIR "/tmp/dtlssrtp"
#define TMP_DIR_TEMPLATE ROOT_TMP_DIR "/XXXXXX"
#define FILE_PERMISIONS (S_IRWXU | S_IRWXG | S_IRWXO)
#define USERNAME_LENGTH 16
#define PASSWORD_LENGTH 24
#define SSRC_LENGTH 8
#define CNAME_LENGTH 16
#define MSLABEL_LENGTH 36
#define TEMPLATE_STRING "TEMPLATE_STRING"
#include "SFUController.h"
#include "XGConfig.h"
#include <uuid/uuid.h>

SFUController::SFUController(const char* serverType)
{
	XGLOG_INFO( "SFUController::SFUController(%s)", serverType);
	memset(m_serverType,0,SERVER_TYPE_SIZE);
	strcpy(m_serverType,serverType);
}

SFUController::~SFUController(void)
{
	XGLOG_INFO( "SFUController::~SFUController(%s) destructor called", m_serverType);
	m_running = false;
}

xGateRetVal SFUController::init (void)
{
	XGLOG_INFO( "SFUController::init(%s)", m_serverType);
	m_gst_init_complete = FALSE;
	// Spawn the thread
	if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
				(void*) this,
				//THR_NEW_LWP|THR_DETACHED) == -1)
		THR_NEW_LWP |THR_BOUND|THR_DETACHED| THR_INHERIT_SCHED) == -1)
		{
			// Spawn Failure
			XGLOG_INFO( "SFUController::init(%s)"\
					" ace error while spawning thread", m_serverType);
			printf("\nSFUController::init spawn failed\n");
			// cleanup the timer manager singelton instance
			return EN_XGATE_STATUS_ACE_ERROR;
		} 
	printf("\nSFUController::init spawn success\n");

	XGLOG_INFO("Waitiing for Gstreamer init completion ");
	while (TRUE != m_gst_init_complete){
		usleep(30000);
	}
	XGLOG_INFO("Gstreamer init success returning form SFU::init");
	return EN_XGATE_STATUS_SUCCESS;  
}

void SFUController::run (void* threadClass)
{
	XGLOG_INFO( "SFUController::run (starting gstreamer events processing loop)");

	SFUController* pGstManager = static_cast<SFUController*>(threadClass);     
	pGstManager->m_running = true; 
	//1. initialize gstreamer library
	if(pGstManager->initGstreamerLib() != EN_XGATE_STATUS_SUCCESS) {
		XGLOG_ERROR( "SFUController::initGstreamerLib failed SFUController thread exit");
		return; 
	}
	pGstManager->m_gst_init_complete = TRUE;
	//Note: below concepts needs to be build before loop
	//2. initialize Gstreamer MainEventLoop from this thread context
	GMainContext *ctx = g_main_context_ref_thread_default();
	GMainLoop *loop = g_main_loop_new (ctx, FALSE);
	pGstManager->gMainLoop = loop;
	//    all the gstreamer related events handled from this thread context. 
	while (pGstManager->m_running)
	{
		//Note: below concepts needs to be build within loop
		g_main_context_iteration (ctx, TRUE); //TRUE - blocking
		//    usleep(2000); //2000 is 2 milliseconds
	}

	XGLOG_INFO( "SFUController::run reactor loop is ended .calling gst_deinit ()");
	//deinitializing gstreamer library
	gst_deinit();
	XGLOG_INFO ("gstreamer de-initialized .. exiting the process by calling exit");
	exit (0);
}

xGateRetVal SFUController::stopThread( void )
{
	XGLOG_INFO( "SFUController::stopThread(%s) called", m_serverType);
	m_running = false;
	XGLOG_INFO( "SFUController::stopThread(%s) Stopped SFUController", m_serverType);
	return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUController::initGstreamerLib()
{
	XGLOG_INFO( "SFUController::initGstreamerLib(%s)", m_serverType);
	GError *err;
	if(!gst_init_check(0, NULL, &err)) {
		if(err) {
			XGLOG_ERROR( "SFUController::initGstreamerLib failed to initialize GStreamer. Error: %d: %s", \
					err->code, err->message); 
			g_error_free(err);
		} else {
			XGLOG_ERROR( "SFUController::initGstreamerLib failed to initialize GStreamer. Unknown Error");
		}
		return EN_XGATE_STATUS_ERROR;
	}

	guint major, minor, micro, nano;
	gst_version (&major, &minor, &micro, &nano);
	XGLOG_INFO( "SFUController::initGstreamerLib successfully initialized Gstreamer version %d.%d.%d-%d", \
			major, minor, micro, nano);

	return EN_XGATE_STATUS_SUCCESS;
}

void SFUController::add_callId_client_conf_map_list(MgMediaDetail &mediaDetail)
{
	XGLOG_INFO("Entered add_callId_client_conf_map_list () client: %p",mediaDetail.pClient);
	SFUController *pSFUController = xGateSFUtil::getSFUController();
	xGateHmpConference *pxGateHmpConfObj = NULL;
	string callId;
	Client *client = NULL;

	if(mediaDetail.mgresource_id.empty() || mediaDetail.call_type == EN_XGATE_CALL_TYPE_UNKNOWN || \
			mediaDetail.call_id.empty() )
	{
		XGLOG_ERROR( "SFUController:: add_callId_client_conf_map_list () Invalid media details !!");
		return;
	}
	pxGateHmpConfObj = pSFUController->find_conference_entry(mediaDetail.mgresource_id);
	if(!pxGateHmpConfObj){
		XGLOG_ERROR("Failed to find entry in conference");
		return;
	}
	callId = mediaDetail.call_Id_msg10;
	client = mediaDetail.pClient;
	if (client){
		XGLOG_INFO("Adding callId: %s client: %p to confMap list",callId.c_str(), client);
		pxGateHmpConfObj->add_client(callId,client);
	}
}

xGateRetVal SFUController::allocate_conf_channel(MgMediaDetail &mediaDetail,xGateNetConnection &pbxConn)
{
	xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
	gboolean isEntryExist = TRUE;
	ClientDetail clientDetail;
	xGateHmpConference *pHmpConference = NULL;
	//pthread_t ptid;

	XGLOG_INFO( "SFUController::allocate_conf_channel for mgresource_id: %s, call_type:%d call_id:%s \
			client_ip:%s client_port:%d codec:%d, sig_type:%d, call_dir %d joiner_type: %d ", mediaDetail.mgresource_id.c_str(), \
			mediaDetail.call_type, mediaDetail.call_id.c_str(), mediaDetail.audioDetail.m_clientIp.c_str(), mediaDetail.audioDetail.m_clientPort, \
			mediaDetail.audioDetail.m_codec, mediaDetail.sig_type, mediaDetail.call_dir, mediaDetail.joiner_type);

	if(mediaDetail.call_type != EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO){
		return EN_XGATE_STATUS_ERROR;
	}

	pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
	if(!pHmpConference){
		pHmpConference = new xGateHmpConference(mediaDetail.mgresource_id, mediaDetail.call_type);
		isEntryExist = FALSE;
	}

	retVal = set_media_details(mediaDetail, clientDetail);

	if(EN_XGATE_STATUS_SUCCESS != retVal) {
		XGLOG_ERROR( "SFUController::allocate_conf_channel failed for mgresource_id: %s, call_type:%d call_id:%s \
				client_ip:%s client_port:%d", mediaDetail.mgresource_id.c_str(), mediaDetail.call_type, \
				mediaDetail.call_id.c_str(), mediaDetail.audioDetail.m_clientIp.c_str(), mediaDetail.audioDetail.m_clientPort);
		return retVal;
	}

	if(pHmpConference){
		if(!isEntryExist){
			XGLOG_INFO("Adding mediaDetail.mgresource_id: %s in conference: %p client_detail_msgresourceId: %s",
					mediaDetail.mgresource_id.c_str(),pHmpConference, clientDetail.m_mgresourceId.c_str());
			insert_conference_entry(mediaDetail.mgresource_id, pHmpConference);
		}
		clientDetail.m_pbxIp = pbxConn.recvIp_;
		clientDetail.m_pbxPort = pbxConn.recvPort_;
		retVal = pHmpConference->create_channel(clientDetail);
	}
	return retVal;
}

void SFUController::handle_conference_release(MgMediaDetail &mediaDetail)
{
	SFUController *pSFUController = xGateSFUtil::getSFUController();
	xGateHmpConference *pxGateHmpConfObj = NULL;
	string callId;
	Client *client = NULL;

	XGLOG_INFO("Entered handle_conference_release () for conference: %s",
			mediaDetail.mgresource_id.c_str());
	if(mediaDetail.mgresource_id.empty() || mediaDetail.call_type == EN_XGATE_CALL_TYPE_UNKNOWN)
	{
		XGLOG_ERROR( "SFUController:: handle_conference_release ()" );
		return;
	}
	pxGateHmpConfObj = pSFUController->find_conference_entry(mediaDetail.mgresource_id);
	if(!pxGateHmpConfObj){
		XGLOG_ERROR("Failed to find entry in conference");
		return;
	}

	client = pxGateHmpConfObj->get_participant_entry(mediaDetail.call_id);
	if (client){
		XGLOG_INFO ("calling pxGateHmpConfObj->remove_conf_participant for callId: %s num_member: %d",
				client->m_detail.m_callId.c_str(), pxGateHmpConfObj->m_clientMap.size());
		pxGateHmpConfObj->remove_conf_participant(client);
	}


	if (pxGateHmpConfObj->m_clientMap.size () == 0){
		pxGateHmpConfObj->m_clientMap.clear();
	}

	if(pxGateHmpConfObj->m_clientMap.size()  == 0){
		if(pxGateHmpConfObj){
			XGLOG_INFO ("remove_conference_entry!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			delete pxGateHmpConfObj;
			remove_conference_entry(mediaDetail.mgresource_id);
		}
	}
}

xGateRetVal SFUController::send_msg_sfu_client(MgMediaDetail *pmg_mediaDetail,
		                               ClientDetail &clientDetail, xGateMgMsgType eMsgType)
{
 XGLOG_INFO("SFUController::send_msg_sfu_client !!!!!!");

 xGateMgMsg sMgMsg;
 xGateNetConnection netConInfo;
 sMgMsg.setMsgType(EN_XGATE_MSG_MGC_MG_IN);
 sMgMsg.set_mg_msg_type(eMsgType);
 sMgMsg.set_media_detail(*pmg_mediaDetail);
 netConInfo.recvIp_ = clientDetail.m_pbxIp;
 netConInfo.recvPort_ = clientDetail.m_pbxPort;
 sMgMsg.set_net_con_info(netConInfo);
 SFUClient *psSFUClient = xGateSFUtil::getSFUClient();

  if (!psSFUClient){
     XGLOG_ERROR("Failed to fetch SFU client");
     return EN_XGATE_STATUS_ERROR;
  }else{
     psSFUClient->send_msg(&sMgMsg);
  }
  return EN_XGATE_STATUS_SUCCESS;
}

void SFUController::send_create_send_channel_request(MgMediaDetail &mediaDetail)
{
	XGLOG_INFO("Entered send_create_send_channel_request() client: %p",mediaDetail.pClient);
	SFUController *pSFUController = xGateSFUtil::getSFUController();
	xGateHmpConference *pxGateHmpConfObj = NULL;
	string callId;
	Client *client = NULL;

	if(mediaDetail.mgresource_id.empty() || mediaDetail.call_type == EN_XGATE_CALL_TYPE_UNKNOWN || \
			mediaDetail.call_id.empty() || !mediaDetail.pClient)
	{
		XGLOG_ERROR( "SFUController:: send_create_send_channel_request() Invalid media details !!");
		return;
	}
	pxGateHmpConfObj = pSFUController->find_conference_entry(mediaDetail.mgresource_id);
	if(!pxGateHmpConfObj){
		XGLOG_ERROR("Failed to find entry in conference");
		return;
	}
	client = mediaDetail.pClient;
	if (client){
		pxGateHmpConfObj->insert_participant_entry(mediaDetail.call_id,client);
		/* free the memory passed*/
	}
}

xGateRetVal SFUController::handle_record_event_msg(MgMediaDetail &mediaDetail)
{
	xGateHmpConference *pHmpConference = NULL;
	Client *client = NULL;
	char *callId_str = NULL;
	XGLOG_ERROR("SFUController::handle_record_event_msg()");

	if(mediaDetail.mgresource_id.empty() || mediaDetail.call_id.empty())
	{
		XGLOG_ERROR("SFUController::handle_record_event_msg() failed due to invalid params!!");
		return EN_XGATE_STATUS_ERROR;
	}
	pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
	if(!pHmpConference){
		XGLOG_ERROR("SFUController::handle_record_event_msg() failed as no conference object failed");
		return EN_XGATE_STATUS_ERROR;
	}
	client = pHmpConference->get_participant_entry(mediaDetail.call_id);
	if(!client){
		XGLOG_ERROR("SFUController::get_participant_entry () failed for call_id: %s",
				mediaDetail.call_id.c_str());
		return EN_XGATE_STATUS_ERROR;
	}

	client->m_detail.m_mediaEvent = mediaDetail.media_event;
	client->m_detail.m_record_file = mediaDetail.record_file;
	client->m_detail.m_browser_type = mediaDetail.browser_type;
	client->m_detail.m_record_url = mediaDetail.record_url;
	client->m_detail.m_record_id = mediaDetail.record_id;
	client->m_detail.m_meeting_name = mediaDetail.meeting_name;
	client->m_detail.m_retrieved_audiossrc = mediaDetail.retrieved_audiossrc;
	client->m_detail.m_retrieved_videossrc = mediaDetail.retrieved_videossrc;
	client->m_detail.m_sip_id = mediaDetail.sip_id;
	client->m_detail.m_isCcaas = mediaDetail.is_ccaas;
	client->m_detail.m_customer_num = mediaDetail.customer_num;
	client->m_detail.m_domain_id = mediaDetail.domain_id;
	client->m_detail.m_agent_id = mediaDetail.agent_id;
	//copy meet user info to client detail
	client->m_detail.meet_userinfo.assign(mediaDetail.meetuser_info.begin(),mediaDetail.meetuser_info.end());

   struct timeval time_now;
   long milli_sec, seconds, useconds;
   gettimeofday(&time_now, NULL);
   seconds = time_now.tv_sec; //seconds
   useconds = time_now.tv_usec; //milliseconds
   milli_sec = ((seconds) * 1000 + useconds/1000.0);


	if(client->m_detail.m_mediaEvent == EN_MEDIA_EVENT_RECORD_STOP)
	{
		client->m_detail.m_recEndTime = milli_sec;
		XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$ EN_MEDIA_EVENT_RECORD_STOP recStartTime:%d and recEndTime:%d",client->m_detail.m_recStartTime, client->m_detail.m_recEndTime);
		handle_conference_release(mediaDetail); //conference channel deletion	
	}

	ACE_Time_Value tval ((time(NULL) ));
	tval += ACE_Time_Value(0,1);
	xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg ();
	if(client->m_detail.m_mediaEvent == EN_MEDIA_EVENT_RECORD_SSRC_REMOVE){
		pMsg->m_msgType = XGATE_CLIENT_SSRC_REMOVE_REQUEST;
	}else{
	pMsg->m_msgType = XGATE_CLIENT_REC_EVENT_REQUEST;
	}
	//pMsg->str_param = mediaDetail.sdp_str;
	pMsg->param1 = static_cast <void*> (client);

	ACE_Message_Block *pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
	if(!client->putq(pAmb, &tval)){
		XGLOG_ERROR("Failed to push Record Event Msg msg to Client thread");
	}
	if (mediaDetail.sdp_str.size()){
		mediaDetail.sdp_str.clear();
	}
	if (mediaDetail.Participant_list.size ()){
		mediaDetail.Participant_list.clear ();
	}
        return EN_XGATE_STATUS_SUCCESS;
}

bool Client::Post_record_info_to_DB(Client* client)
{
  string strContext = client->m_detail.m_callId;
  //Reading sp name from /root/mbc.conf config file
  //std::string spName = CONFIGSERVICE->m_dbModProfile.m_sp_name;

  std::string spName("");
  string strData("");
	DBServiceEvent event = EN_DB_EVENT_UNKNOWN;
	std::map<EN_DB_MSG_TYPE, DBConnInfo*>::iterator itr;
	if(client->m_detail.m_isCcaas)
	{
	  itr = XG_CONFIG::instance()->m_dbModProfile.m_mapDbConnInfo.find(EN_DB_MSG_TYPE::EN_DB_MSG_TYPE_CCAAS_CALL_RECORD);
    event = EN_DB_EVENT_STORE_CCAAS_VIDEO_REC_DATA_QUERY;
	  strData = Encode_ccaas_meet_record_client_info();
	}
	else
	{
	  itr = XG_CONFIG::instance()->m_dbModProfile.m_mapDbConnInfo.find(EN_DB_MSG_TYPE::EN_DB_MSG_TYPE_UCAAS_CALL_RECORD);
    event = EN_DB_EVENT_STORE_VIDEO_REC_DATA_QUERY;
	  strData = client->Encode_meet_record_client_info();
	}

	if(itr != XG_CONFIG::instance()->m_dbModProfile.m_mapDbConnInfo.end())
	{
		DBConnInfo* dbConnInfo = itr->second;
    spName = dbConnInfo->m_sp_name.c_str();
	}


  xGateDBServiceMsg *ptDbMsg = FormDBRequestMsg(strContext, spName, strData, event);
  if (!ptDbMsg)
  {
        XGLOG_ERROR("OOM Issue. Creating DB request info for Server info failed");
        return false;
  }

  XGLOG_INFO("Sending EN_DB_EVENT_STORE_VIDEO_REC_DATA_QUERY event to DB Module");
  return SFUSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(ptDbMsg));
}

string Client::Encode_meet_record_client_info()
{
	XGLOG_INFO("Client::Encode_Record_Client_Info Start for client '%s' ",m_detail.m_callId.c_str());
	Client *client = this;
	string meeting_id = m_detail.m_mgresourceId;
	string sipid = m_detail.m_sip_id;
	string recordfile = m_detail.m_record_file;
	string url = m_detail.m_record_url;
	string meeting_name = m_detail.m_meeting_name;
	string FileSize = m_detail.m_rec_fileSize;
  	string Duration = m_detail.m_file_duration;

	// Generate random uuid number
	uuid_t uuid;
	uuid_generate_random(uuid);
	char uuid_str[37];
	uuid_unparse(uuid, uuid_str);
	string unique_id = uuid_str;

	string jsonStr = "'" + meeting_id + "','" + meeting_name + "','" + sipid + "','" + unique_id + "','" +recordfile + "','" + url + "','" + FileSize + "','" + Duration + "',";
	// string EncodedString = "'" + encode_meet_user_info_record() + "'";
	string EncodedString = "'" + encode_meet_user_info_record() + "'";
	string FullJson = jsonStr + EncodedString;
	XGLOG_INFO(" =>meeting_Id:'%s', meeting_name: '%s', sip_Id: '%s', unique_id: '%s', recordfile: '%s, url: '%s', 'filesize '%s' file duration '%s' meetUserInfo '%s'",
						meeting_id.c_str(), meeting_name.c_str(), sipid.c_str(), unique_id.c_str(), recordfile.c_str(), url.c_str(), FileSize.c_str(), Duration.c_str(),EncodedString.c_str());
	return FullJson;
}

string Client::Encode_ccaas_meet_record_client_info()
{
	XGLOG_INFO("Client::Encode_ccaas_meet_record_client_info for client '%s' ",m_detail.m_callId.c_str());
	Client *client = this;
	string meeting_id = m_detail.m_mgresourceId;
	string session_id = m_detail.m_record_id;
	int domain_id = m_detail.m_domain_id;
	int agent_id = m_detail.m_agent_id;
	string customer_num = m_detail.m_customer_num;
	string recordfile = m_detail.m_record_file;
	string url = m_detail.m_record_url;
	string meeting_name = m_detail.m_meeting_name;
	string file_size = m_detail.m_rec_fileSize;
	string thumbnail_url("");
  string duration = m_detail.m_file_duration;
	long int start_time = m_detail.m_recStartTime;
	long int end_time = m_detail.m_recEndTime;
	int call_dir = 1;

  string jsonStr("");
	std::ostringstream oss;
  oss << "'" << meeting_id << "'," << domain_id << ",'" << session_id << "','" << url << "','" << recordfile << "','" \
      << file_size << "','""','" << start_time << "','" << end_time << "','" << duration << "','" << call_dir << "','" \
      << agent_id << "','" << customer_num << "'";

  jsonStr = oss.str();
  XGLOG_INFO(" => Encode_ccaas_meet_record_client_info : %s", jsonStr.c_str());
  return jsonStr;
}

string Client::encode_meet_user_info_record()
{
	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	rapidjson::Value users(rapidjson::kArrayType);
	for (const auto& user : m_detail.meet_userinfo) {
		rapidjson::Value u(rapidjson::kObjectType);
		u.AddMember("email", rapidjson::Value().SetString(user.email.c_str(), allocator), allocator);
		u.AddMember("profile_img", rapidjson::Value().SetString(user.profile_img.c_str(), allocator), allocator);
		u.AddMember("sip_id", rapidjson::Value().SetString(user.sip_id.c_str(), allocator), allocator);
		u.AddMember("name", rapidjson::Value().SetString(user.name.c_str(), allocator), allocator);
		u.AddMember("ext", rapidjson::Value().SetString(user.ext.c_str(), allocator), allocator);
		u.AddMember("user_type", rapidjson::Value().SetString(user.user_type.c_str(), allocator), allocator);
		u.AddMember("role_type", rapidjson::Value().SetInt(user.role_type), allocator);
		users.PushBack(u, allocator);
	}
	document.AddMember("urmeet_user_list", users, allocator);
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);
	std::string json_string = buffer.GetString();
	//Get the Array value without Key "urmeet_user_list"
	unsigned int pos = json_string.find("[");
  	int len = json_string.length()-(pos+1);
  	string finalStr = json_string.substr(pos,len);
	std::cout << "meetuserinfo json: " << finalStr << std::endl;
	return finalStr;
}

xGateDBServiceMsg * Client::FormDBRequestMsg(string &strContext, string &spName, string &strData, DBServiceEvent event)
{
    XGLOG_DEBUG("Inside Client::FormDBRequestMsg");
    xGateDBServiceMsg * ptdbMsg = new xGateDBServiceMsg();
    if (!ptdbMsg)
    {
            XGLOG_ERROR("OOM Issue. Creating DB request failed");
            return NULL;
    }

    ptdbMsg->setDstModuleId(IURDefines::UR_MODULE_DB_SERVICE);
    ptdbMsg->setSrcModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
    ptdbMsg->setDbMsgType(xGateDBServiceMsg::DB_SRV_MSG_REQ);
    DBRequestInfo &dbReqInfo                = ptdbMsg->get_db_request_info();
    dbReqInfo.m_contextId                   = strContext.c_str();
    dbReqInfo.m_dbEvent                     = event;
    dbReqInfo.m_requestorModule             = IURDefines::UR_MODULE_SFU_SERVICE;
    dbReqInfo.m_spName                      = spName.c_str();
    dbReqInfo.m_data                        = strData.c_str();

    return ptdbMsg;
}

xGateRetVal SFUController::release_channel(MgMediaDetail &mediaDetail)
{
	xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
	xGateHmpConference *pHmpConference = NULL;
	Client *client = NULL;

	XGLOG_INFO( "SFUController::release_channel for mgresource_id:%s, call_type:%d call_id:%s \
			client_ip:%s client_port:%d codec:%d, sig_type:%d, call_dir %d", mediaDetail.mgresource_id.c_str(), \
			mediaDetail.call_type, mediaDetail.call_id.c_str(), mediaDetail.audioDetail.m_clientIp.c_str(), mediaDetail.audioDetail.m_clientPort, \
			mediaDetail.audioDetail.m_codec, mediaDetail.sig_type, mediaDetail.call_dir);

	if(mediaDetail.mgresource_id.empty()) {
		XGLOG_ERROR( "SFUController::release_channel failed. Invalid media details !!");
		return retVal;
	}
	/* Check if the release channel is for conference or one-one call */
	if((mediaDetail.call_type == EN_XGATE_CALL_TYPE_CONFERENCE)
			|| (mediaDetail.call_type == EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO)){
		pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
		if(pHmpConference) {
			delete pHmpConference;
			remove_conference_entry(mediaDetail.mgresource_id);
			retVal = EN_XGATE_STATUS_SUCCESS;
		}else {
			XGLOG_ERROR("release_channel failed. pHmpConference object not found for mgresource_id:%s",
					mediaDetail.mgresource_id.c_str());
		}
	}
	return retVal;
}

// Start Conference MAP related interfaces
xGateHmpConference* SFUController::find_conference_entry(string conferenceId)
{
	xGateHmpConference* pHmpConference = NULL;
	if(m_hmpConfMap.find(conferenceId.c_str(), pHmpConference) != -1) {
		// XGLOG_INFO( "find_conference_entry success for conference_Id: %s", conferenceId.c_str());
	} else {
		XGLOG_ERROR( "find_conference_entry failed for conference_Id: %s", conferenceId.c_str());
	}
	return pHmpConference;
}

xGateRetVal SFUController::insert_conference_entry(string conferenceId, xGateHmpConference* pHmpConference)
{
	xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
	int status = -1;

	status = m_hmpConfMap.bind(conferenceId.c_str(), pHmpConference);
	if(status == 0) {
		XGLOG_INFO( "insert_conference_entry success for conferenceId: %s", conferenceId.c_str());
		retVal = EN_XGATE_STATUS_SUCCESS;
	} else if(status == 1) {
		XGLOG_INFO( "insert_conference_entry attempt is made to bind an existing entry for conferenceId: %s", conferenceId.c_str());
	} else if(status == -1) {
		XGLOG_ERROR( "insert_conference_entry failed for conferenceId: %s", conferenceId.c_str());
	}
	return retVal;
}

xGateRetVal SFUController::remove_conference_entry(string conferenceId)
{
	XGLOG_INFO( "Entered remove_conference_entry for conferenceId: %s", conferenceId.c_str());
	m_hmpConfMap.unbind(conferenceId.c_str());
        return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUController::set_media_details(MgMediaDetail &mediaDetail, ClientDetail &clientDetail)
{
	clientDetail.m_mgresourceId = mediaDetail.mgresource_id;
	clientDetail.m_callType = mediaDetail.call_type;
	clientDetail.m_callId = mediaDetail.call_id;
	clientDetail.m_legId = (mediaDetail.leg_id > 0) ? (mediaDetail.leg_id-1) : 0;
	clientDetail.m_callDir = mediaDetail.call_dir;
	clientDetail.m_retrieved_audiossrc = mediaDetail.retrieved_audiossrc;
	clientDetail.m_retrieved_videossrc = mediaDetail.retrieved_videossrc;
	clientDetail.m_sigType = mediaDetail.sig_type;
	clientDetail.m_isCcaas = mediaDetail.is_ccaas;
	clientDetail.m_mediaEvent = mediaDetail.media_event;
	clientDetail.m_record_file = mediaDetail.record_file;
	clientDetail.m_browser_type = mediaDetail.browser_type;
	clientDetail.m_record_url = mediaDetail.record_url;
	clientDetail.m_sip_id = mediaDetail.sip_id;
	clientDetail.m_customer_num = mediaDetail.customer_num;
	clientDetail.m_domain_id = mediaDetail.domain_id;
	clientDetail.m_agent_id = mediaDetail.agent_id;
	clientDetail.m_sdpStr = mediaDetail.sdp_str;
	clientDetail.m_joinerType = mediaDetail.joiner_type;
	clientDetail.m_screenshare_id = mediaDetail.screenshare_id;
	clientDetail.audioDetail.m_mediaType = mediaDetail.audioDetail.m_mediaType;
	clientDetail.audioDetail.m_clientIp = mediaDetail.audioDetail.m_clientIp;
	mediaDetail.audioDetail.m_clientIp = "";
	clientDetail.audioDetail.m_clientPort = mediaDetail.audioDetail.m_clientPort;
	mediaDetail.audioDetail.m_clientPort = 0;
	clientDetail.audioDetail.m_codec = mediaDetail.audioDetail.m_codec;
	clientDetail.audioDetail.m_dynamicCodec = mediaDetail.audioDetail.m_dynamicCodec;
	clientDetail.audioDetail.m_codecName = mediaDetail.audioDetail.m_codecName;
	clientDetail.audioDetail.m_ptime = mediaDetail.audioDetail.m_ptime;

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

	//TODO: these code block condition's are bit confusing, we need to review this later
	if((clientDetail.m_callDir == EN_CALL_DIR_IN) && \
			(clientDetail.m_mediaModeIn == EN_MEDIA_DTLS || clientDetail.m_mediaModeOut == EN_MEDIA_DTLS)) {
		//    generateLocalSecureDetails(clientDetail);
	}
	// Moving this section out as SIMPLE_VIDEO may or maynot be DTLS/CALL_DIR_IN
	if (clientDetail.m_callType == EN_XGATE_CALL_TYPE_SIMPLE_VIDEO || clientDetail.m_callType == EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO) {
		clientDetail.videoDetail.m_mediaType = mediaDetail.videoDetail.m_mediaType;
		clientDetail.videoDetail.m_clientIp = mediaDetail.videoDetail.m_clientIp;
		mediaDetail.videoDetail.m_clientIp = "";
		clientDetail.videoDetail.m_clientPort = mediaDetail.videoDetail.m_clientPort;
		mediaDetail.videoDetail.m_clientPort = 0;
		clientDetail.videoDetail.m_codec = mediaDetail.videoDetail.m_codec;
		clientDetail.videoDetail.m_dynamicCodec = mediaDetail.videoDetail.m_dynamicCodec;
		clientDetail.videoDetail.m_codecName = mediaDetail.videoDetail.m_codecName;
		clientDetail.videoDetail.m_frameRate = mediaDetail.videoDetail.m_frameRate;
		clientDetail.videoDetail.m_imageattr_x = mediaDetail.videoDetail.m_imageattr_x;
		clientDetail.videoDetail.m_imageattr_y = mediaDetail.videoDetail.m_imageattr_y;
	}

	return EN_XGATE_STATUS_SUCCESS;

}

xGateRetVal SFUController::update_media_details(ClientDetail &clientDetail, MgMediaDetail &mediaDetail)
{
	mediaDetail.audioDetail.m_mediaType = clientDetail.audioDetail.m_mediaType;
	mediaDetail.audioDetail.m_clientIp = clientDetail.audioDetail.m_clientIp;
	mediaDetail.audioDetail.m_clientPort = clientDetail.audioDetail.m_clientPort;
	mediaDetail.audioDetail.m_dialIp = mediaDetail.audioDetail.m_respIp = clientDetail.audioDetail.m_serverIp;
	mediaDetail.audioDetail.m_respPort = clientDetail.audioDetail.m_serverPort;
	mediaDetail.audioDetail.m_ptime = clientDetail.audioDetail.m_ptime;

	if(clientDetail.m_mediaModeIn == EN_MEDIA_DTLS || clientDetail.m_mediaModeOut == EN_MEDIA_DTLS) {
		if (clientDetail.m_callType == EN_XGATE_CALL_TYPE_SIMPLE_VIDEO) {
			mediaDetail.videoDetail.m_mediaType = clientDetail.videoDetail.m_mediaType;
			mediaDetail.videoDetail.m_clientIp = clientDetail.videoDetail.m_clientIp;
			mediaDetail.videoDetail.m_clientPort = clientDetail.videoDetail.m_clientPort;
			mediaDetail.videoDetail.m_dialIp = mediaDetail.videoDetail.m_respIp = clientDetail.videoDetail.m_serverIp;
			mediaDetail.videoDetail.m_respPort = clientDetail.videoDetail.m_serverPort;
			mediaDetail.videoDetail.m_frameRate = clientDetail.videoDetail.m_frameRate;
			mediaDetail.videoDetail.m_imageattr_x = clientDetail.videoDetail.m_imageattr_x;
			mediaDetail.videoDetail.m_imageattr_y = clientDetail.videoDetail.m_imageattr_y;
		}
	}
	return EN_XGATE_STATUS_SUCCESS;
}

