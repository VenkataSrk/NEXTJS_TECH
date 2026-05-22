
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
#include "xGateHmpThreadPool.h"
#include "SFUController.h"
#define TRUE 1
#define FALSE 0

SFUController::SFUController(const char* serverType)
{
  XGLOG_INFO( "SFUController::SFUController(%s)", serverType);
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);

#ifdef RECORD_ENABLED
  m_rcvd_video_handler = NULL;
  m_rcvd_audio_handler = NULL;
  m_record_video_handler = NULL;
  m_record_audio_handler = NULL;
  m_hmpConf = NULL;
#endif
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

  // create different type thread pools to process data and events
  // threadpool type, thread creation type exclusive (true/false)
  // if true threads are created when thread pool is created,
  // false threds are shared between other thred pools and created
  // depending on need

  this->m_ThreadPool_info.m_rx_data_pool = NULL;
  this->m_ThreadPool_info.m_event_pool = NULL;
  this->m_ThreadPool_info.m_decoder_pool = NULL;
  this->m_ThreadPool_info.m_encoder_pool = NULL;

  this->m_ThreadPool_info.create_thread_pool (this, THREAD_POOL_EVENT, TRUE);
  this->m_ThreadPool_info.create_thread_pool (this, THREAD_POOL_RX_DATA, TRUE);
  this->m_ThreadPool_info.create_thread_pool (this, THREAD_POOL_AUDIO_DEC, TRUE);
  // The number of threads is found to increase if we make audio encoder threads
  // to share with other thread pool since the number of threads in thread pool
  // is not configured to have fixed number of threads some where in gstreamer (which
  // creates the first thread pool
  this->m_ThreadPool_info.create_thread_pool (this, THREAD_POOL_AUDIO_ENC, TRUE);

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
  XGLOG_INFO( "SFUController::successfully initialized Gstreamer version %d.%d.%d-%d", \
      major, minor, micro, nano);

  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUController::send_msg_sfu_client(MgMediaDetail *pmg_mediaDetail,
		                               ClientDetail &clientDetail,
					       xGateMgMsgType eMsgType)
{
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

void SFUController::send_msg_clients_gen_offers(void *arg)
{

 ClientDetail *clientDetail = (ClientDetail*)(arg);
 xGateHmpConference *pHmpConference = NULL;
 HmpFrameChannel *pHmpFrameChannel = NULL;
 HmpFrameBin *pHmpFrameBin = NULL;
 Client *client = NULL;
 xGateHmpClientThreadMsg *pMsg = NULL;
 SFUController *pSFUController = xGateSFUtil::getSFUController();
 XGLOG_INFO("Sending Message to clients to generate offers of new Client, callId:%s ,\
	      conf_id:%s joiner_type: %d ",clientDetail->m_callId.c_str(),
	                  clientDetail->m_mgresourceId.c_str(), clientDetail->m_joinerType);
 pHmpConference = pSFUController->find_conference_entry(clientDetail->m_mgresourceId);
 if(!pHmpConference){
   XGLOG_ERROR("==== Failed to find conference object for mgresourceId: %s Not generating offers",
		    clientDetail->m_mgresourceId.c_str());
   return NULL;
 }else if(clientDetail->m_mgresourceId.empty()) {
   XGLOG_ERROR("mgresourceId: is NULL Not generating offers");
   return NULL;
 }
 /* New client needs to be extracted */
 client = pHmpConference->get_participant_entry(clientDetail->m_callId);
 if(!client){
   XGLOG_ERROR("Could not find client in genOffers for callId:%s",clientDetail->m_callId.c_str());
   return NULL;
 }

 XGLOG_INFO("Generating offer's TO Existing clients");
 /* Add new client to existing clients */
 for(auto itr = pHmpConference->m_clientMap.begin();itr != pHmpConference->m_clientMap.end();++itr){
   Client *existingClient = itr->second;
   if(existingClient == client){
     continue;
   }
   /* Now post message to client thread to create send webrtcBin */
   ACE_Time_Value tval ((time(NULL) ));
   tval += ACE_Time_Value(0,1);
   char *callId_str = NULL;
   int *joiner_type = NULL;
   ACE_Message_Block *pAmb = NULL;
   pMsg = new xGateHmpClientThreadMsg ();
   pMsg->m_msgType = XGATE_CLIENT_CREATE_OFFER;
   callId_str = malloc(strlen(client->m_detail.m_callId.c_str())+1);
   strcpy (callId_str, client->m_detail.m_callId.c_str());

   pMsg->param1 = static_cast <void*> (callId_str);
   pMsg->param2 = static_cast <void*> (existingClient);
   pMsg->str_param = client->m_FrameChannel->m_pHmpFrameBin->m_localSdpStr;
   pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
   XGLOG_INFO("OFFER from new client: %s TO existing client %s ",
		   callId_str, existingClient->m_detail.m_callId.c_str());
   pMsg->pClient = existingClient;
   if (pSFUController->m_ThreadPool_info.m_event_pool) {
     GError **error = nullptr;
     g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, error);
     if (error != nullptr){
       XGLOG_ERROR ("Failed to push event to thread pool");
     }
   }
   else{
     XGLOG_ERROR ("event pool thread is null... can't push event");
   }
 }
 /* Add existing clients to new client */
 XGLOG_INFO("Generating offer's  TO New client");
 for(auto itr = pHmpConference->m_clientMap.begin();itr != pHmpConference->m_clientMap.end();++itr){
   Client *existingClient = itr->second;
   if(existingClient == client){
      continue;
   }
   /* Now post message to client thread to create send webrtcBin */
   ACE_Time_Value tval ((time(NULL) ));
   tval += ACE_Time_Value(0,1);
   char *callId_str = NULL;
   int *joiner_type = NULL;
   ACE_Message_Block *pAmb = NULL;
   pMsg = new xGateHmpClientThreadMsg ();

   pMsg->m_msgType = XGATE_CLIENT_CREATE_OFFER;
   callId_str = malloc(strlen(existingClient->m_detail.m_callId.c_str())+1);
   strcpy (callId_str, existingClient->m_detail.m_callId.c_str());

   pMsg->str_param = existingClient->m_FrameChannel->m_pHmpFrameBin->m_localSdpStr;
   pMsg->param1 = static_cast <void*> (callId_str);
   pMsg->param2 = static_cast <void*> (client);
   pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
   XGLOG_INFO("==== OFFER from existing client ( %s ) TO NEW client ( %s )",
		   callId_str, client->m_detail.m_callId.c_str());

   pMsg->pClient = client;
   if (pSFUController->m_ThreadPool_info.m_event_pool) {
    GError **error = nullptr;
    g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, error);
    if (error != nullptr){
       XGLOG_ERROR ("Failed to push event to thread pool");
     }
   }
   else{
     XGLOG_ERROR ("event pool thread is null... can't push event");
   }
 }
}


void SFUController::send_key_frame_request (MgMediaDetail &mediaDetail)
{
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  xGateHmpConference *pxGateHmpConfObj = NULL;

  if(mediaDetail.mgresource_id.empty() || mediaDetail.call_type == EN_XGATE_CALL_TYPE_UNKNOWN || \
      mediaDetail.call_id.empty() )
  {
      XGLOG_ERROR( "SFUController:: send_key_frame_request failed. Invalid media details !!");
      return;
  }
//  XGLOG_INFO("Sending key frame request to client: %s", mediaDetail.call_id.c_str());
  pxGateHmpConfObj = pSFUController->find_conference_entry(mediaDetail.mgresource_id);
  if(!pxGateHmpConfObj){
     XGLOG_ERROR("Failed to find entry in conference");
     return;
  }
  for(auto itr = pxGateHmpConfObj->m_clientMap.begin();
		  itr != pxGateHmpConfObj->m_clientMap.end();++itr){
      Client *dst_client = itr->second;
      if (dst_client && (0 == strcmp ( mediaDetail.call_id.c_str(), dst_client->m_detail.m_callId.c_str()))) {
        xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg;
        if (pMsg){
	  ACE_Message_Block* pAmb = NULL;
	  ACE_Time_Value tval ((time(NULL) ));
	  tval += ACE_Time_Value(0,1);
          pMsg->m_msgType = XGATE_CLIENT_SEND_KEY_FRAME_REQUEST;
          pMsg->param1 = static_cast <void*> (dst_client);
          pMsg->pClient = dst_client;
          if (pSFUController->m_ThreadPool_info.m_event_pool) {
             GError **error = nullptr;
             g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, error);
             if (error != nullptr){
                 XGLOG_ERROR ("Failed to push event to thread pool");
             }
           }
           else{
             XGLOG_ERROR ("event pool thread is null... can't push event");
           }
        }
      break;
     }
  }
}

xGateRetVal SFUController::allocate_conf_channel(MgMediaDetail &mediaDetail,xGateNetConnection &pbxConn)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  gboolean isEntryExist = TRUE;
  ClientDetail clientDetail;
  xGateHmpConference *pHmpConference = NULL;
  pthread_t ptid;
  XGLOG_INFO( "SFUController::allocate_conf_channel for mgresource_id: %s, call_type:%d call_id:%s \
      joiner_type: %d ",
      mediaDetail.mgresource_id.c_str(), \
      mediaDetail.call_type, mediaDetail.call_id.c_str(), mediaDetail.joiner_type);

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
    XGLOG_ERROR( "SFUController::allocate_conf_channel failed for mgresource_id: %s, call_type:%d call_id:%s",
        mediaDetail.mgresource_id.c_str(), mediaDetail.call_type,
        mediaDetail.call_id.c_str());
    return retVal;
  }

  if(pHmpConference){
    if(!isEntryExist){
       XGLOG_INFO("Adding mgresource_id: %s in conference: %p",
		    mediaDetail.mgresource_id.c_str(),pHmpConference);
       insert_conference_entry(mediaDetail.mgresource_id, pHmpConference);
    }
#ifdef RECORD_ENABLED
    m_hmpConf = pHmpConference;
    if(!spawn_media_handler_pool()) {
	XGLOG_ERROR("SFUController::allocate_conf_channel failed. Spawning media handler pool failed !");
	return retVal;
    }
#endif
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
  pxGateHmpConfObj->m_StopConference = TRUE;

  CLIENT_MAP::iterator itr = pxGateHmpConfObj->m_clientMap.begin();
  while(itr != pxGateHmpConfObj->m_clientMap.end()) {
     Client *client = itr->second;
    /*
     * Post message to every client to stop
     */
     if (client){
#ifdef RECORD_ENABLED
       HmpRecordSendChannel *channel = pxGateHmpConfObj->get_record_send_channel(client->m_detail.m_callId);
       if(channel != NULL){
         client->handle_send_stop_recording(client, channel);
         pxGateHmpConfObj->remove_record_send_channel(client->m_detail.m_callId);
         XGLOG_INFO("SFUController::deleted recording channel for client: %s", client->m_detail.m_callId.c_str());
       }
#endif
     }
     itr++;
  }
  if (pxGateHmpConfObj->m_clientMap.size ()){
     pxGateHmpConfObj->m_clientMap.clear();
  }
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
    send_msg_clients_gen_offers((void*) &client->m_detail);
    /* free the memory passed*/
  }
}

xGateRetVal SFUController::handle_active_speaker_request_msg(MgMediaDetail &mediaDetail)
{
  XGLOG_INFO("Handling active speaker request for  call_id: %s",mediaDetail.call_id.c_str());
  xGateHmpConference *pHmpConference = NULL;
  Client *client = NULL;
  PARTICIPANT_LIST *pPartList = nullptr;
  if(mediaDetail.mgresource_id.empty() || mediaDetail.call_id.empty())
  {
    XGLOG_ERROR("SFUController::handle_active_speaker_request_msg failed due to invalid params!!");
    return EN_XGATE_STATUS_ERROR;
  }
  pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
  if(!pHmpConference){
    XGLOG_ERROR("SFUController::handle_active_speaker_request_msg failed as no conference object failed");
    return EN_XGATE_STATUS_ERROR;
  }
  client = pHmpConference->get_participant_entry(mediaDetail.call_id);
  if(!client){
    XGLOG_ERROR("active_speaker_request_msg get_participant_entry () failed for call_id: %s",
                                         mediaDetail.call_id.c_str());
    return EN_XGATE_STATUS_ERROR;
  }

  client->handle_active_speaker_request (pPartList);

  pHmpConference->send_active_speaker_info (pHmpConference->m_activeSpeakerId, TRUE);

  if(mediaDetail.sdp_str.size()){
    mediaDetail.sdp_str.clear();
  }
  if(mediaDetail.Participant_list.size ()){
    mediaDetail.Participant_list.clear ();
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUController::handle_grid_view_request_msg (MgMediaDetail &mediaDetail)
{
  XGLOG_INFO("Entered handle_grid_view_request_msg() for  call_id: %s",mediaDetail.call_id.c_str());
  xGateHmpConference *pHmpConference = NULL;
  Client *client = NULL;
  PARTICIPANT_LIST *pPartList = nullptr;
  if(mediaDetail.mgresource_id.empty() || mediaDetail.call_id.empty())
  {
    XGLOG_ERROR("SFUController::handle_grid_view_request_msg() failed due to invalid params!!");
    return EN_XGATE_STATUS_ERROR;
  }
  pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
  if(!pHmpConference){
    XGLOG_ERROR("SFUController::handle_grid_view_request_msg() failed as no conference object failed");
    return EN_XGATE_STATUS_ERROR;
  }
  client = pHmpConference->get_participant_entry(mediaDetail.call_id);
  if(!client){
    XGLOG_ERROR("handle_grid_view_request_msg() get_participant_entry () failed for call_id: %s",
                                         mediaDetail.call_id.c_str());
    return EN_XGATE_STATUS_ERROR;
  }

  /*view Type is grid view */
  client->m_viewType = XGATE_VIEW_TYPE_GRID;
  XGLOG_INFO ("AFTER procesing grid_view_request() client->m_mid3CallId: %s viewType: %u",
                      client->m_mid3CallId.c_str(), client->m_mid3CallType, client->m_viewType);

  if(mediaDetail.sdp_str.size()){
    mediaDetail.sdp_str.clear();
  }
  if(mediaDetail.Participant_list.size ()){
    mediaDetail.Participant_list.clear ();
  }

  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUController::handle_screenshare_request_msg(MgMediaDetail &mediaDetail)
{
  XGLOG_INFO("Handling screenshare request for  call_id: %s",mediaDetail.call_id.c_str());
  xGateHmpConference *pHmpConference = NULL;
   SFUController *pSFUController = xGateSFUtil::getSFUController();
  Client *client = NULL;
  if(mediaDetail.mgresource_id.empty() || mediaDetail.call_id.empty())
  {
    XGLOG_ERROR("SFUController::handle_screenshare_request_msg failed due to invalid params!!");
    return EN_XGATE_STATUS_ERROR;
  }
  pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
  if(!pHmpConference){
    XGLOG_ERROR("SFUController::handle_screenshare_request_msg failed as no conference object failed");
    return EN_XGATE_STATUS_ERROR;
  }
  client = pHmpConference->get_participant_entry(mediaDetail.call_id);
  if(!client){
    XGLOG_ERROR("screenshare_request_msg get_participant_entry () failed for call_id: %s",
		                    mediaDetail.call_id.c_str());
    return EN_XGATE_STATUS_ERROR;
  }
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg ();
  pMsg->m_msgType = XGATE_CLIENT_SCREENSHARE_REQUEST;
  pMsg->str_param = mediaDetail.sdp_str;
  pMsg->param2 = static_cast <void*> (client);
  ParticipantInfo pPInfo, pPartInfo;
  PARTICIPANT_LIST *pPartList = new (PARTICIPANT_LIST);
  for (int i = 0;i < mediaDetail.Participant_list.size (); i++)
  {
    pPInfo = mediaDetail.Participant_list.at(i);
    if(pPartList){
      pPartList->emplace (pPartList->begin() + i, pPInfo);
    }
  }
  pMsg->param1 = static_cast <void*> (pPartList);
  pMsg->pClient = client;
  if (pSFUController->m_ThreadPool_info.m_event_pool) {
    GError **error = nullptr;
    g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, error);
    if (error != nullptr){
       XGLOG_ERROR ("Failed to push event to thread pool");
     }
  }
  else{
     XGLOG_ERROR ("event pool thread is null... can't push event");
  }

  if(mediaDetail.sdp_str.size()){
    mediaDetail.sdp_str.clear();
  }
  if(mediaDetail.Participant_list.size ()){
    mediaDetail.Participant_list.clear ();
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUController::handle_tile_change_request_msg(MgMediaDetail &mediaDetail)
{
  XGLOG_INFO("Handling tile change change request for  call_id: %s",mediaDetail.call_id.c_str());
  xGateHmpConference *pHmpConference = NULL;

  Client *client = NULL;
  if(mediaDetail.mgresource_id.empty() || mediaDetail.call_id.empty())
  {
    XGLOG_ERROR("SFUController::handle_tile_change_request_msg failed due to invalid params!!");
    return EN_XGATE_STATUS_ERROR;
  }
  pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
  if(!pHmpConference){
    XGLOG_ERROR("SFUController::handle_tile_change_request_msg failed as no conference object failed");
     return EN_XGATE_STATUS_ERROR;
  }
  client = pHmpConference->get_participant_entry(mediaDetail.call_id);
  if(!client){
    XGLOG_ERROR("tile_change_request_msg get_participant_entry () failed for call_id: %s",
                        mediaDetail.call_id.c_str());
     return EN_XGATE_STATUS_ERROR;
  }
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg ();
  pMsg->m_msgType = XGATE_CLIENT_TILE_CHANGE_REQUEST;
  pMsg->str_param = mediaDetail.sdp_str;
  pMsg->param2 = static_cast <void*> (client);
  ParticipantInfo pPInfo, pPartInfo;
  PARTICIPANT_LIST *pPartList = new (PARTICIPANT_LIST);
  for (int i = 0;i < mediaDetail.Participant_list.size (); i++)
  {
     pPInfo = mediaDetail.Participant_list.at(i);
     if (pPartList){
       pPartList->emplace (pPartList->begin() + i, pPInfo);
     }
  }
  pMsg->param1 = static_cast <void*> (pPartList);

  if(client->rcv_data_msg_q){
     ACE_Message_Block *pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
     if (!client->rcv_data_msg_q->enqueue_prio(pAmb, &tval))
       XGLOG_ERROR("Failed to push XGATE_CLIENT_TILE_CHANGE_REQUEST msg to Client thread");
  } else if (this->m_ThreadPool_info.m_rx_data_pool) {
     GError  *error = NULL;
     g_mutex_lock (&client->m_data_process_lock);

     pMsg->pClient = client;
     g_thread_pool_push (this->m_ThreadPool_info.m_rx_data_pool, pMsg, &error);

     if (error != NULL){
       XGLOG_ERROR ("Failed to push Tile change event to thread pool");
       client->ullNextVideoMsgNum--;
       g_error_free (error);
     }
     g_mutex_unlock (&client->m_data_process_lock);
  }
  if (mediaDetail.sdp_str.size()){
     mediaDetail.sdp_str.clear();
  }
  if (mediaDetail.Participant_list.size ()){
    mediaDetail.Participant_list.clear ();
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUController::handle_sdp_update_request_msg(MgMediaDetail &mediaDetail)
{
  xGateHmpConference *pHmpConference = NULL;
  Client *client = NULL;
  HmpFrameChannel *pHmpFrameChannel = NULL;
  HmpFrameBin *pHmpFrameBin = NULL;
  char *callId_str = NULL;
  gboolean bAudoOnlyInserted = FALSE;
  gboolean bScreenShareInserted = FALSE;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  int i = 0, j = 0;
  if(mediaDetail.mgresource_id.empty() || mediaDetail.call_id.empty())
  {
    XGLOG_ERROR("SFUController::handle_sdp_update_request_msg() failed due to invalid params!!");
    return EN_XGATE_STATUS_ERROR;
  }
  pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
  if(!pHmpConference){
    XGLOG_ERROR("SFUController::handle_sdp_update_request_msg() failed as no conference object failed");
     return EN_XGATE_STATUS_ERROR;
  }
  client = pHmpConference->get_participant_entry(mediaDetail.call_id);
  if(!client){
    XGLOG_ERROR("SFUController::get_participant_entry () failed for call_id: %s",
		                    mediaDetail.call_id.c_str());
     return EN_XGATE_STATUS_ERROR;
  }
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg ();
  pMsg->m_msgType = XGATE_CLIENT_SDP_UPDATE_REQUEST;
  pMsg->str_param = mediaDetail.sdp_str;
  pMsg->param2 = static_cast <void*> (client);
  ParticipantInfo pPInfo, pPartInfo;
  PARTICIPANT_LIST *pPartList = new (PARTICIPANT_LIST);
  /* Find if callid of the client for which SDP update request is received is
   * present in the list. then check for audio_only, screenshare call_id.
   * if there are present , copy those call_id at the beginning of the list
   */
  for (i = 0, j = 0;i < mediaDetail.Participant_list.size (); i++)
  {
     pPInfo = mediaDetail.Participant_list.at(i);
     if (pPartList){
       if (0 == strcmp (pPInfo.call_id.c_str(), client->m_detail.m_callId.c_str())){
          pPartList->emplace (pPartList->begin() + j, pPInfo);
          j++;
        } else if (!bAudoOnlyInserted && (0 == strcmp (pPInfo.call_id.c_str(), "audio_only"))) {
          pPartList->emplace (pPartList->begin() + j, pPInfo);
          bAudoOnlyInserted = TRUE;
          j++;
        } else if (!bScreenShareInserted && (0 == strcmp (pPInfo.call_id.c_str(), "screenshare"))) {
          pPartList->emplace (pPartList->begin() + j, pPInfo);
          bScreenShareInserted = TRUE;
          j++;
        }
     }
  }
  /* Now copy other clients present in the list */
  for ( i = 0; i < mediaDetail.Participant_list.size (); i++) {
     pPInfo = mediaDetail.Participant_list.at(i);
     if (pPartList){
       if (0 != strcmp (pPInfo.call_id.c_str(), client->m_detail.m_callId.c_str())
           && (0 != strcmp (pPInfo.call_id.c_str(), "audio_only"))
           && (0 != strcmp (pPInfo.call_id.c_str(), "screenshare"))){
          pPartList->emplace (pPartList->begin() + j, pPInfo);
          j++;
        }
     }
  }

  pMsg->param1 = static_cast <void*> (pPartList);
  pMsg->pClient = client;
  if (pSFUController->m_ThreadPool_info.m_event_pool) {
    GError **error = nullptr;
    g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, error);
    if (error != nullptr){
       XGLOG_ERROR ("Failed to push event to thread pool");
     }
  }
  else{
     XGLOG_ERROR ("event pool thread is null... can't push event");
  }
  if (mediaDetail.sdp_str.size()){
     mediaDetail.sdp_str.clear();
  }
  if (mediaDetail.Participant_list.size ()){
    mediaDetail.Participant_list.clear ();
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUController::handle_sdp_update_response_msg(MgMediaDetail &mediaDetail)
{
  xGateHmpConference *pHmpConference = NULL;
  Client *client = NULL;
  HmpFrameChannel *pHmpFrameChannel = NULL;
  HmpFrameBin *pHmpFrameBin = NULL;
  if(mediaDetail.mgresource_id.empty() || mediaDetail.call_id.empty())
  {
    XGLOG_ERROR("SFUController::handle_sdp_update_response_msg() failed due to invalid params!!");
    return EN_XGATE_STATUS_ERROR;
  }
  pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
  if(!pHmpConference){
    XGLOG_ERROR("SFUController::handle_sdp_update_response_msg() failed as no conference object failed");
     return EN_XGATE_STATUS_ERROR;
  }
  client = pHmpConference->get_participant_entry (mediaDetail.call_id);
  if(!client){
    XGLOG_ERROR("SFUController::get_client() failed for call_id: %s",
		                    mediaDetail.call_id.c_str());
     return EN_XGATE_STATUS_ERROR;
  }
  pHmpFrameChannel = client->m_FrameChannel;
  pHmpFrameBin = pHmpFrameChannel->m_pHmpFrameBin;
  pHmpFrameBin->m_remoteSdpStr = mediaDetail.sdp_str;
  pHmpFrameBin->handle_sdp_answer(client);
  return EN_XGATE_STATUS_SUCCESS;
}

#ifdef RECORD_ENABLED
xGateRetVal SFUController::handle_record_event_req(MgMediaDetail &mediaDetail)
{
  xGateHmpConference *pHmpConference = NULL;
  Client *client = NULL;
  char *callId_str = NULL;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  if(mediaDetail.mgresource_id.empty() || mediaDetail.call_id.empty())
  {
    XGLOG_ERROR("SFUController::handle_record_event_req() failed due to invalid params!!");
    return EN_XGATE_STATUS_ERROR;
  }
  pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
  if(!pHmpConference){
    XGLOG_ERROR("SFUController::handle_record_event_req() failed as no conference object failed");
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
  client->m_detail.m_meeting_name = mediaDetail.meeting_name;
  client->m_detail.m_sip_id = mediaDetail.sip_id;
  client->m_detail.meet_userinfo.assign(mediaDetail.meetuser_info.begin(),mediaDetail.meetuser_info.end());

  if(client->m_detail.m_mediaEvent == EN_MEDIA_EVENT_RECORD_STOP) {
    XGLOG_INFO("SFUController::handle rec req for client:%s with event EN_MEDIA_EVENT_RECORD_STOP",mediaDetail.call_id.c_str());
    HmpRecordSendChannel *channel = pHmpConference->get_record_send_channel(mediaDetail.call_id);
    if(channel) {
      pHmpConference->remove_record_send_channel(mediaDetail.call_id);
      XGLOG_INFO("SFUController::deleted recording channel %s entry from record map",mediaDetail.call_id.c_str());
      client->handle_send_stop_recording(client, channel);
    }
    return EN_XGATE_STATUS_SUCCESS;
  }
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg ();
  pMsg->m_msgType = XGATE_CLIENT_REC_EVENT_REQUEST;
  pMsg->param1 = static_cast <void*> (client);
  ACE_Message_Block *pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
  pMsg->pClient = client;
  if (pSFUController->m_ThreadPool_info.m_event_pool) {
    GError **error = nullptr;
    g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, error);
    if (error != nullptr){
       XGLOG_ERROR ("Failed to push event to thread pool");
     }
  }
  else{
     XGLOG_ERROR ("event pool thread is null... can't push event");
  }
  if (mediaDetail.sdp_str.size()){
     mediaDetail.sdp_str.clear();
  }
  if (mediaDetail.Participant_list.size ()){
     mediaDetail.Participant_list.clear ();
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUController::handle_record_event_res(MgMediaDetail &mediaDetail)
{
  xGateHmpConference *pHmpConference = NULL;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  Client *client = NULL;
  char *callId_str = NULL;
  if(mediaDetail.mgresource_id.empty() || mediaDetail.call_id.empty())
  {
    XGLOG_ERROR("SFUController::handle_record_event_res() failed due to invalid params!!");
    return EN_XGATE_STATUS_ERROR;
  }
  pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
  if(!pHmpConference){
    XGLOG_ERROR("SFUController::handle_record_event_res() failed as no conference object failed");
    return EN_XGATE_STATUS_ERROR;
  }
  client = pHmpConference->get_participant_entry(mediaDetail.call_id);
  if(!client){
    XGLOG_ERROR("SFUController::get_participant_entry () failed for call_id: %s",
		                    mediaDetail.call_id.c_str());
    return EN_XGATE_STATUS_ERROR;
  }
  client->m_detail.m_mediaEvent = mediaDetail.media_event;
  client->m_detail.audioDetail = mediaDetail.audioDetail;
  client->m_detail.videoDetail = mediaDetail.videoDetail;

  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg ();
  pMsg->m_msgType = XGATE_CLIENT_REC_EVENT_RESPONSE;
  pMsg->param1 = static_cast <void*> (client);
  pMsg->pClient = client;
  if (pSFUController->m_ThreadPool_info.m_event_pool) {
    GError **error = nullptr;
    g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, error);
    if (error != nullptr){
       XGLOG_ERROR ("Failed to push event to thread pool");
     }
  }
  else{
     XGLOG_ERROR ("event pool thread is null... can't push event");
  }
  if (mediaDetail.sdp_str.size()){
     mediaDetail.sdp_str.clear();
  }
  if (mediaDetail.Participant_list.size ()){
    mediaDetail.Participant_list.clear ();
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUController::handle_record_keyframe_request(MgMediaDetail &mediaDetail)
{
  xGateHmpConference *pHmpConference = NULL;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  Client *client = NULL;
  char *callId_str = NULL;
  if(mediaDetail.mgresource_id.empty() || mediaDetail.call_id.empty())
  {
    XGLOG_ERROR("SFUController::handle_record_event_res() failed due to invalid params!!");
    return EN_XGATE_STATUS_ERROR;
  }
  pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
  if(!pHmpConference){
    XGLOG_ERROR("SFUController::handle_record_event_res() failed as no conference object failed");
    return EN_XGATE_STATUS_ERROR;
  }
  client = pHmpConference->get_ssrc_participant_entry(mediaDetail.retrieved_videossrc);
  if(!client){
    XGLOG_ERROR("SFUController::get_ssrc_participant_entry () failed for call_id: %s",
                        mediaDetail.call_id.c_str());
    return EN_XGATE_STATUS_ERROR;
  }
  client->m_detail.m_mediaEvent = mediaDetail.media_event;
  client->m_detail.audioDetail = mediaDetail.audioDetail;
  client->m_detail.videoDetail = mediaDetail.videoDetail;

  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg ();
  pMsg->m_msgType = XGATE_CLIENT_SEND_KEY_FRAME_REQUEST;
  pMsg->param1 = static_cast <void*> (client);
#if 0
  ACE_Message_Block *pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
  if(!client->putq(pAmb, &tval)){
    XGLOG_ERROR("Failed to push Record Event Msg msg to Client thread");
  }
#endif
  pMsg->pClient = client;
  if (pSFUController->m_ThreadPool_info.m_event_pool) {
    GError **error = nullptr;
    g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, error);
    if (error != nullptr){
       XGLOG_ERROR ("Failed to push event to thread pool");
     }
  }
  else{
     XGLOG_ERROR ("event pool thread is null... can't push event");
  }
  if (mediaDetail.sdp_str.size()){
     mediaDetail.sdp_str.clear();
  }
  if (mediaDetail.Participant_list.size ()){
    mediaDetail.Participant_list.clear ();
  }
  return EN_XGATE_STATUS_SUCCESS;
}

#endif

void SFUController::retry_media_connection_request (MgMediaDetail &mediaDetail)
{
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  xGateHmpConference *pxGateHmpConfObj = NULL;
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  Client *client = NULL;
  xGateHmpClientThreadMsg *pMsg = NULL;
  ACE_Message_Block *pAmb = 0;
  HmpFrameBin *pHmpFrameBin = NULL;

  if(mediaDetail.mgresource_id.empty() || mediaDetail.call_type == EN_XGATE_CALL_TYPE_UNKNOWN || \
      mediaDetail.call_id.empty() )
  {
      XGLOG_ERROR( "SFUController:: retry_media_connection_request failed. Invalid media details !!");
      return;
  }
  XGLOG_INFO("Entered retry_media_connection_request () mg_resource: %s callId: %s",
		  mediaDetail.mgresource_id.c_str(), mediaDetail.call_id.c_str());
  pxGateHmpConfObj = pSFUController->find_conference_entry(mediaDetail.mgresource_id);
  if(!pxGateHmpConfObj){
     XGLOG_ERROR("Failed to find entry in conference");
     return;
  }
  client = pxGateHmpConfObj->get_participant_entry(mediaDetail.call_id);
  if(!client){
    XGLOG_ERROR("SFUController::get_participant_entry() failed for call_id: %s",
		                    mediaDetail.call_id.c_str());
    return EN_XGATE_STATUS_ERROR;
  }
  //retVal = set_media_details(NULL, mediaDetail, client->m_detail);

  if(EN_XGATE_STATUS_SUCCESS != retVal) {
    XGLOG_ERROR( "SFUController:: retry_media_connection_request () failed for mgresource_id: %s,"
        "call_id:%s client_ip:%s client_port:%d",
        mediaDetail.mgresource_id.c_str(),
        mediaDetail.call_id.c_str(), mediaDetail.audioDetail.m_clientIp.c_str(),
	mediaDetail.audioDetail.m_clientPort);
  }
  XGLOG_INFO("DTLS connection failed for RECV channel .. Deleting RECV channel");
  if (client->m_FrameChannel){
     pHmpFrameBin = client->m_FrameChannel->m_pHmpFrameBin;
  }
}

xGateRetVal  SFUController::media_delete_channel(MgMediaDetail &mediaDetail)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  Client *client = NULL;
  xGateHmpConference *pHmpConference = NULL;
  xGateCallType callType = EN_XGATE_CALL_TYPE_UNKNOWN;

  XGLOG_INFO( "SFUController::media_delete_channel for mgresource_id:%s, call_type:%d call_id:%s \
       call_dir %d", mediaDetail.mgresource_id.c_str(), \
      mediaDetail.call_type, mediaDetail.call_id.c_str(), mediaDetail.call_dir);

  if(mediaDetail.mgresource_id.empty()) {
    XGLOG_ERROR( "SFUController::media_delete_channel failed. Invalid media details !!");
    return retVal;
  }

  callType = mediaDetail.call_type;
  switch(callType) {
     case EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO:
     {
       HmpFrameChannel *pHmpFrameChannel = NULL;
       HmpFrameBin *pHmpFrameBin = NULL;
       XGLOG_INFO("media_delete_channel EN_XGATE_CALL_TYPE_CONFERENCE for call_id: %s",mediaDetail.call_id.c_str());
       pHmpConference = find_conference_entry(mediaDetail.mgresource_id);
       if(!pHmpConference){
          XGLOG_ERROR("media_delete_channel for confId:%s failed",mediaDetail.mgresource_id);
          return EN_XGATE_STATUS_ERROR;
       }
       Client *client =  pHmpConference->get_participant_entry(mediaDetail.call_id);
       if(!client){
          XGLOG_ERROR("media_delete_channel for callId:%s failed",mediaDetail.call_id.c_str());
          return EN_XGATE_STATUS_ERROR;
       }
       // if client type is screen share and it's disconnected, we need to assign the
       // mid3Type callid as active speaker since we need to send active speaker data in
       // mid3 line
       if ((client->m_detail.m_joinerType == EN_JOINER_TYPE_SCREENSHARE)
           || (!client->m_detail.m_screenshare_id.empty())) {
         Client *dst_client = NULL;
         for (auto itr = pHmpConference->m_clientMap.begin ();
                itr != pHmpConference->m_clientMap.end (); ++itr){
           dst_client = itr->second;
           if (dst_client){
              dst_client->m_mid3CallType = XGATE_MID3_CALL_TYPE_ACTIVE_SPEAKER;
           }
         }
       }

       if (0 == strcmp (client->m_detail.m_callId.c_str(), mediaDetail.call_id.c_str())){
          pHmpConference->remove_conf_participant(client);
       }else {
          XGLOG_ERROR("Could not find client in list for callId: %s",mediaDetail.call_id.c_str());
       }

       if (!pHmpConference->m_Prev_activeSpeakerId.empty() &&
            (pHmpConference->m_Prev_activeSpeakerId == mediaDetail.call_id) &&
            (pHmpConference->m_activeSpeakerId != mediaDetail.call_id)){
           XGLOG_INFO ("clearing the Previous Active speaker %s as m_Prev_activeSpeakerId == call_id.c_str",
                 pHmpConference->m_Prev_activeSpeakerId.c_str());
           pHmpConference->m_Prev_activeSpeakerId.erase();
           pHmpConference->send_active_speaker_info (pHmpConference->m_activeSpeakerId, FALSE);
       }

       //Need to make active speaker Id as NULL because Active Speaker Video will be sent to Recording Server
       //If Active Speaker Id is NULL local video will be sent to Recording Server until next active speaker info came
       if(!pHmpConference->m_activeSpeakerId.empty()){
         if(0 == strcmp(pHmpConference->m_activeSpeakerId.c_str(), mediaDetail.call_id.c_str())){
           XGLOG_INFO ("Current Active Speaker of Conf is deleted making conf ActiveSpeakerId as NULL");
           //check if previous active speaker callId and assign to active speaker
           if (pHmpConference->m_Prev_activeSpeakerId != mediaDetail.call_id){
              pHmpConference->m_activeSpeakerId.erase ();
              XGLOG_INFO ("Sending Prev active spkr: %s to Client as new active spkr",
                             pHmpConference->m_Prev_activeSpeakerId.c_str());
              pHmpConference->send_active_speaker_info (pHmpConference->m_Prev_activeSpeakerId, FALSE);
           }else{
             pHmpConference->m_activeSpeakerId = "";
           }
         }
       }
      HmpRecordSendChannel *channel = pHmpConference->get_record_send_channel(mediaDetail.call_id);
       if(channel) {
         pHmpConference->remove_record_send_channel(mediaDetail.call_id);
         XGLOG_INFO("SFUController::deleted recording channel for client: %s",mediaDetail.call_id.c_str());
         client->handle_send_stop_recording(client, channel);
       }
       break;
     }
    case EN_XGATE_CALL_TYPE_UNKNOWN:
    default:
      {
        XGLOG_ERROR( "media_delete_channel failed! unsupported / unknown call type for call_id: %s",
                             mediaDetail.call_id.c_str());
        return EN_XGATE_STATUS_ERROR;
      }
  }

  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUController::release_channel(MgMediaDetail &mediaDetail)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  xGateHmpConference *pHmpConference = NULL;
  Client *client = NULL;

  XGLOG_INFO( "SFUController::release_channel for mgresource_id:%s, call_type:%d call_id:%s \
       call_dir %d", mediaDetail.mgresource_id.c_str(),
      mediaDetail.call_type, mediaDetail.call_id.c_str(),
       mediaDetail.call_dir);

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
xGateHmpConference* SFUController::find_conference_entry(string &conferenceId)
{
  xGateHmpConference* pHmpConference = NULL;

  HMP_CONF_MAP::iterator it;
  it = m_hmpConfMap.find(conferenceId);
  if (it != m_hmpConfMap.end())
    pHmpConference = it->second;
  return pHmpConference;
}

xGateRetVal SFUController::insert_conference_entry(string conferenceId, xGateHmpConference* pHmpConference)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  int status = -1;
  m_hmpConfMap.insert({conferenceId, pHmpConference});
  retVal = EN_XGATE_STATUS_SUCCESS;
  return retVal;
}

xGateRetVal SFUController::remove_conference_entry(string conferenceId)
{
  XGLOG_INFO( "Entered remove_conference_entry for conferenceId: %s", conferenceId.c_str());
  //m_hmpConfMap.unbind(conferenceId.c_str());
  m_hmpConfMap.erase(conferenceId);
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUController::set_media_details(MgMediaDetail &mediaDetail, ClientDetail &clientDetail)
{
  clientDetail.m_mgresourceId = mediaDetail.mgresource_id;
  clientDetail.m_callType = mediaDetail.call_type;
  clientDetail.m_callId = mediaDetail.call_id;
  clientDetail.m_record_url = mediaDetail.record_url;
  clientDetail.m_meeting_name = mediaDetail.meeting_name;
  clientDetail.m_sip_id = mediaDetail.sip_id;
  clientDetail.meet_userinfo = mediaDetail.meetuser_info;
  clientDetail.m_conn_id = mediaDetail.conn_id;
  clientDetail.m_device_id = mediaDetail.device_id;
  clientDetail.m_legId = (mediaDetail.leg_id > 0) ? (mediaDetail.leg_id-1) : 0;
  clientDetail.m_callDir = mediaDetail.call_dir;
  clientDetail.m_sigType = mediaDetail.sig_type;
  clientDetail.m_mediaEvent = mediaDetail.media_event;
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
  clientDetail.m_asrType = mediaDetail.asr_type;
  clientDetail.is_ccaas = mediaDetail.is_ccaas;
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


#ifdef RECORD_ENABLED
gboolean SFUController::spawn_media_handler_pool()
{
  if(!m_rcvd_video_handler) {
    m_rcvd_video_handler = new SFUClientRcvdVideoHandler(m_hmpConf, 1, FALSE);
  }

  if(!m_rcvd_audio_handler) {
    m_rcvd_audio_handler = new SFUClientRcvdAudioHandler(m_hmpConf, 1, FALSE);
  }

  if(!m_record_video_handler) {
    m_record_video_handler = new SFUClientRecordVideoHandler(m_hmpConf, 1, FALSE);
  }

  if(!m_record_audio_handler) {
    m_record_audio_handler = new SFUClientRecordAudioHandler(m_hmpConf, 1, FALSE);
  }
  return TRUE;
}

SFUClientRcvdVideoHandler* SFUController::get_rcvd_video_handler()
{
  return m_rcvd_video_handler;
}

SFUClientRcvdAudioHandler* SFUController::get_rcvd_audio_handler()
{
  return m_rcvd_audio_handler;
}

SFUClientRecordVideoHandler* SFUController::get_record_video_handler()
{
  return m_record_video_handler;
}

SFUClientRecordAudioHandler* SFUController::get_record_audio_handler()
{
  return m_record_audio_handler;
}
#endif
