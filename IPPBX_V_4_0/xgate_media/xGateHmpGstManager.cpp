#include "xgconfig.h"
#ifdef XGATE_HAS_GSTREAMER 

//local include
#include "xglog.h"

//self include
#include "xGateHmpGstManager.h"

#define THISMODULE "GstManag"
/**
 * Default Constructor
 */
//TODO: time configuration must be dynamic
xGateHmpGstManager::xGateHmpGstManager(const char* serverType)
{
  XGLOG_INFO( "xGateHmpGstManager::xGateHmpGstManager(%s)", serverType);
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);
}

/**
 * Destructor
 */
xGateHmpGstManager::~xGateHmpGstManager (void)
{
  XGLOG_INFO( "xGateHmpGstManager::~xGateHmpGstManager(%s) destructor called", m_serverType);
  m_running = false;
}//end destructor

/**
 * Initialize the xGateHmpGstManager
 */
xGateRetVal xGateHmpGstManager::init (void)
{

  XGLOG_INFO( "xGateHmpGstManager::init(%s)", m_serverType);

  // Spawn the thread
  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1)
  {
    // Spawn Failure
    XGLOG_INFO( "xGateHmpGstManager::init(%s)"\
        " ace error while spawning thread", m_serverType);
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
  XGLOG_INFO( "xGateHmpGstManager::run (starting gstreamer events processing loop)");

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
    usleep(2000); //2000 is 2 milliseconds
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
  XGLOG_INFO( "xGateHmpGstManager::stopThread(%s) called", m_serverType);
  m_running = false;
  XGLOG_INFO( "xGateHmpGstManager::stopThread(%s) Stopped xGateHmpGstManager", m_serverType);
  return EN_XGATE_STATUS_SUCCESS;
} 

xGateRetVal xGateHmpGstManager::initGstreamerLib()
{
  XGLOG_INFO( "xGateHmpGstManager::initGstreamerLib(%s)", m_serverType);
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


xGateRetVal xGateHmpGstManager::allocate_receive_channel(MgMediaDetail &mediaDetail, xGateNetConnection &pbxConn)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  gboolean isEntryExist = TRUE;
  ClientDetail clientDetail;
  xGateHmpCall *pHmpCall = NULL;

  XGLOG_INFO( "xGateHmpGstManager::allocate_channel for mgresource_id: %s, call_type:%d call_id:%s client_ip:%s client_port:%d codec:%d, sig_type:%d, call_dir %d", (char*)ur_log_string(mediaDetail.mediaResource_id), mediaDetail.call_type, (char*)ur_log_string(mediaDetail.call_id), (char*)ur_log_string(mediaDetail.sdpinfo[0].ip_addr), mediaDetail.sdpinfo[0].port, mediaDetail.sdpinfo[0].codec, mediaDetail.sig_type, mediaDetail.call_dir);

  if(mediaDetail.mediaResource_id.empty() || mediaDetail.call_type == EN_XGATE_CALL_TYPE_UNKNOWN || \
      mediaDetail.call_id.empty() || mediaDetail.sdpinfo[0].ip_addr.empty() || mediaDetail.sdpinfo[0].port <= 0) {
    XGLOG_ERROR( "xGateHmpGstManager::allocate_channel failed. Invalid media details !!");
    return retVal;
  }

  //1. Check whether the Call object created and its entry available in Map using call_id
  pHmpCall = find_call_entry(mediaDetail.mediaResource_id);
  if(!pHmpCall) { //first time creating call entry for this call_id
    pHmpCall = new xGateHmpCall(mediaDetail.mediaResource_id, mediaDetail.call_type); 
    isEntryExist = FALSE;
  }

  retVal = set_media_details(pHmpCall, mediaDetail, clientDetail);
  if(EN_XGATE_STATUS_SUCCESS != retVal) {
    XGLOG_ERROR( "xGateHmpGstManager::allocate_channel failed for mgresource_id: %s, call_type:%d call_id:%s \
        client_ip:%s client_port:%d", (char*)ur_log_string(mediaDetail.mediaResource_id), mediaDetail.call_type, \
        (char*)ur_log_string(mediaDetail.call_id), (char*)ur_log_string(mediaDetail.sdpinfo[0].ip_addr), mediaDetail.sdpinfo[0].port);
    return retVal;
  }

  if(pHmpCall) {
    //fill pbx connection info in clientDetail itself
    clientDetail.m_pbxIp = pbxConn.recvIp_;  
    clientDetail.m_pbxPort = pbxConn.recvPort_;
    retVal = pHmpCall->create_receive_channel(clientDetail); 
    //on success add this call entry into map(callid vs hmp call object)
    if(retVal == EN_XGATE_STATUS_SUCCESS) {
      if(!isEntryExist) {
        add_call_entry(mediaDetail.mediaResource_id, pHmpCall);
      }
      update_media_details(clientDetail, mediaDetail);  
    }
  }
  XGLOG_INFO( " \n Created Message : mgresource_id: %s, call_type:%d call_id:%s \
        resp_ip:%s resp_port:%d", (char*)ur_log_string(mediaDetail.mediaResource_id), mediaDetail.call_type, \
        (char*)ur_log_string(mediaDetail.call_id), (char*)ur_log_string(mediaDetail.sdpinfo[0].resp_ip), mediaDetail.sdpinfo[0].resp_port);

  return retVal;
}

xGateRetVal xGateHmpGstManager::connect_receive_channel(MgMediaDetail &mediaDetail)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  xGateHmpCall *pHmpCall = NULL;
  unsigned int legId = 0;
  string record_file ="";

  XGLOG_INFO( "xGateHmpGstManager::connect_channel for mgresource_id: %s, call_id:%s \ 
      client_ip:%s client_port:%d codec:%d,", (char*)ur_log_string(mediaDetail.mediaResource_id), \
      (char*)ur_log_string(mediaDetail.call_id), (char*)ur_log_string(mediaDetail.sdpinfo[0].ip_addr), mediaDetail.sdpinfo[0].port, \
      mediaDetail.sdpinfo[0].codec);

  if(mediaDetail.mediaResource_id.empty() ||  mediaDetail.call_id.empty()) {
    XGLOG_ERROR( "xGateHmpGstManager::connect_channel failed. Invalid media details !!");
    return retVal;
  }

  //1. Check whether the Call object created and its entry available in Map using call_id
  pHmpCall = find_call_entry(mediaDetail.mediaResource_id);
  if(!pHmpCall) {
    XGLOG_ERROR( "connect_channel failed. HmpCall object not found for call_id:%s", (char*)ur_log_string(mediaDetail.call_id));
    return retVal;
  }

  if (pHmpCall->m_clientList.size() < 2) {  
     XGLOG_ERROR( "connect_channel failed. HmpCall object contains one legs for resource_id:%s", (char*)ur_log_string(mediaDetail.mediaResource_id));
     return retVal;
  }

  legId = (mediaDetail.leg_id > 0) ? (mediaDetail.leg_id-1) : 0;
  record_file ="/recordfile.wav";

  //retVal = pHmpCall->start_call_record(record_file, legId);//TODO: Suve need to check
  retVal = pHmpCall->start_call_record(record_file, mediaDetail.call_id);
  return retVal;
}

xGateRetVal xGateHmpGstManager::release_receive_channel(MgMediaDetail &mediaDetail)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  xGateHmpCall *pHmpCall = NULL;
  Client *client = NULL;

  XGLOG_INFO( "xGateHmpGstManager::release_channel for mgresource_id:%s, call_type:%d call_id:%s \ 
      client_ip:%s client_port:%d codec:%d sig_type:%d call_dir %d", (char*)ur_log_string(mediaDetail.mediaResource_id), \
      mediaDetail.call_type, (char*)ur_log_string(mediaDetail.call_id), (char*)ur_log_string(mediaDetail.sdpinfo[0].ip_addr), mediaDetail.sdpinfo[0].port, \
      mediaDetail.sdpinfo[0].codec, mediaDetail.sig_type, mediaDetail.call_dir);

  if(mediaDetail.mediaResource_id.empty()) {
    XGLOG_ERROR( "xGateHmpGstManager::release_channel failed. Invalid media details !!");
    return retVal;
  }

  //1. Check whether the Call object created and its entry available in Map using mgresource_id
  pHmpCall = find_call_entry(mediaDetail.mediaResource_id);
  if(!pHmpCall) {
      XGLOG_ERROR( "release_channel failed. HmpCall object not found for mgresource_id:%s", (char*)ur_log_string(mediaDetail.mediaResource_id));
      return retVal;
  } else {
    
    //pHmpCall->stop_call_record(mediaDetail.call_id);
    delete pHmpCall;
    erase_call_entry(mediaDetail.mediaResource_id);
    retVal = EN_XGATE_STATUS_SUCCESS;
  } 
  return retVal;
}

xGateRetVal xGateHmpGstManager::media_delete_channel(MgMediaDetail &mediaDetail)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  xGateHmpCall *pHmpCall = NULL;
  Client *client = NULL;

  XGLOG_INFO( "xGateHmpGstManager::media_delete_channel for mgresource_id:%s, call_id:%s ", \
      (char*)ur_log_string(mediaDetail.mediaResource_id), (char*)ur_log_string(mediaDetail.call_id));

  if(mediaDetail.mediaResource_id.empty()) {
    XGLOG_ERROR( "xGateHmpGstManager::media_delete_channel failed. Invalid media details !!");
    return retVal;
  }

  pHmpCall = find_call_entry(mediaDetail.mediaResource_id);
  if(!pHmpCall) {
    XGLOG_ERROR( "media_delete_channel failed. HmpCall object not found for mgresource_id:%s", (char*)ur_log_string(mediaDetail.mediaResource_id));
    return retVal;
  }

  client = pHmpCall->get_client_entry(mediaDetail.call_id);
  if(client) {
    pHmpCall->stop_call_record(mediaDetail.call_id);
    pHmpCall->remove_client_entry(client);
  } else {
    XGLOG_ERROR( "media_delete_channel failed. client object not found for call_id: %s",(char*)ur_log_string(mediaDetail.call_id));
  }
}

xGateRetVal xGateHmpGstManager::handle_call_record(MgMediaDetail &mediaDetail)
{
  XGLOG_INFO( "handle_call_record for mgresource_id:%s call_id: %s start", \
      (char*)ur_log_string(mediaDetail.mediaResource_id), (char*)ur_log_string(mediaDetail.call_id));
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  xGateMediaEvent mediaEvent = EN_MEDIA_EVENT_NULL;
  xGateHmpCall *pHmpCall = NULL;
  xGateCallType callType = EN_XGATE_CALL_TYPE_UNKNOWN;  

  pHmpCall = find_call_entry(mediaDetail.mediaResource_id);
  if(!pHmpCall) {
    XGLOG_ERROR( "handle_call_record failed. no call entry for mgresource_id: %s !!", (char*)ur_log_string(mediaDetail.mediaResource_id));
    return EN_XGATE_STATUS_ERROR;
  }
  callType = mediaDetail.call_type;
  mediaEvent = mediaDetail.pb_state;
  switch(mediaEvent) 
  {
    case EN_MEDIA_EVENT_RECORD_START:
      {
        XGLOG_INFO( "handle_call_record event EN_MEDIA_EVENT_RECORD_START for call_id: %s", (char*)ur_log_string(mediaDetail.call_id));
	if(mediaDetail.record_file.empty()) { 
	  XGLOG_ERROR("hadel_call_record failed. Record file path not specified for call_id %s !!", (char*)ur_log_string(mediaDetail.call_id));
          return EN_XGATE_STATUS_ERROR; 
	}
        retVal = pHmpCall->start_call_record(mediaDetail.record_file, mediaDetail.call_id);
        //based on this response event, pbx can play some announcement or perform business logics
        if(retVal == EN_XGATE_STATUS_SUCCESS) {
          mediaDetail.pb_state = EN_MEDIA_EVENT_RECORD_SUCCESS; 
        } else {
          mediaDetail.pb_state = EN_MEDIA_EVENT_RECORD_FAILED; 
        }
        break;
      }
    case EN_MEDIA_EVENT_RECORD_STOP:
      {
        XGLOG_INFO( "handle_call_record event EN_MEDIA_EVENT_RECORD_STOP for call_id: %s", (char*)ur_log_string(mediaDetail.call_id));
        retVal = pHmpCall->stop_call_record(mediaDetail.call_id);
        if(retVal == EN_XGATE_STATUS_SUCCESS) {
          mediaDetail.pb_state = EN_MEDIA_EVENT_RECORD_SUCCESS;

          } else {
          mediaDetail.pb_state = EN_MEDIA_EVENT_RECORD_FAILED;
        }
        //Note: here we don't need to change the media_event. by default pbx will consider this as success
        break;
      }
    case EN_MEDIA_EVENT_RECORD_HOLD:
      {
        XGLOG_INFO( "handle_call_record event EN_MEDIA_EVENT_RECORD_HOLD for call_id: %s", (char*)ur_log_string(mediaDetail.call_id));
	if(callType == EN_XGATE_CALL_TYPE_AUDIO) {
          retVal = pHmpCall->hold_call_record(mediaDetail.call_id);
	} else if(callType == EN_XGATE_CALL_TYPE_AUDIO_CONF) {
          retVal = pHmpCall->hold_conf_call_record(mediaDetail.call_id);
	}
        if(retVal == EN_XGATE_STATUS_SUCCESS) {
          mediaDetail.pb_state = EN_MEDIA_EVENT_RECORD_SUCCESS;
        } else {
          mediaDetail.pb_state = EN_MEDIA_EVENT_RECORD_FAILED;
        }
        break;
      }
    case EN_MEDIA_EVENT_RECORD_RESUME:
      {
        XGLOG_INFO( "handle_call_record event EN_MEDIA_EVENT_RECORD_RESUME for call_id: %s", (char*)ur_log_string(mediaDetail.call_id));
	if(callType == EN_XGATE_CALL_TYPE_AUDIO) {\
          retVal = pHmpCall->unhold_call_record(mediaDetail.call_id);
	} else if(callType == EN_XGATE_CALL_TYPE_AUDIO_CONF) {
	  retVal = pHmpCall->unhold_conf_call_record(mediaDetail.call_id);
        }
        if(retVal == EN_XGATE_STATUS_SUCCESS) {
          mediaDetail.pb_state = EN_MEDIA_EVENT_RECORD_SUCCESS;
        } else {
          mediaDetail.pb_state = EN_MEDIA_EVENT_RECORD_FAILED;
        }
        break;
      }
    case EN_MEDIA_EVENT_NULL:
    default:
      {
        XGLOG_INFO( "handle_call_record failed! unsupported call record event for call_id: %s !!", (char*)ur_log_string(mediaDetail.call_id));
        return EN_XGATE_STATUS_ERROR;
      }
  }
  return retVal;
}

xGateHmpCall * xGateHmpGstManager::find_call_entry(string callId)
{
  xGateHmpCall *pHmpCall = NULL;
  if(m_hmpCallMap.find(callId.c_str(), pHmpCall) != -1) {
    XGLOG_INFO( "find_call_entry success for mgresource_id: %s", (char*)ur_log_string(callId));
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
    XGLOG_INFO( "add_call_entry success for mgresource_id: %s", (char*)ur_log_string(callId));
    retVal = EN_XGATE_STATUS_SUCCESS;
  } else if(status == 1) {
    XGLOG_INFO( "add_call_entry attempt is made to bind an existing entry for mgresource_id: %s", (char*)ur_log_string(callId));
  } else if(status == -1) {
    XGLOG_ERROR( "add_call_entry failed for mgresource_id: %s", (char*)ur_log_string(callId));
  }
  return retVal;
}

xGateRetVal xGateHmpGstManager::erase_call_entry(string callId)
{
  XGLOG_INFO( "erase_call_entry_in_map for mgresource_id: %s", (char*)ur_log_string(callId));
  m_hmpCallMap.unbind(callId.c_str());
}


xGateRetVal xGateHmpGstManager::set_media_details(xGateHmpCall *pHmpCall, MgMediaDetail &mediaDetail, ClientDetail &clientDetail)
{
  clientDetail.m_mgresourceId = mediaDetail.mediaResource_id;
  clientDetail.m_callType = mediaDetail.call_type;
  clientDetail.m_callId = mediaDetail.call_id;
  clientDetail.m_legId = (mediaDetail.leg_id > 0) ? (mediaDetail.leg_id-1) : 0;
  clientDetail.m_callDir = mediaDetail.call_dir;
  clientDetail.m_sigType = mediaDetail.sig_type;
  clientDetail.m_mediaEvent = mediaDetail.pb_state;

  clientDetail.audioDetail.m_mediaType = mediaDetail.sdpinfo[0].mediaType;
  clientDetail.audioDetail.m_clientIp = mediaDetail.sdpinfo[0].ip_addr;
  mediaDetail.sdpinfo[0].ip_addr = "";
  clientDetail.audioDetail.m_clientPort = mediaDetail.sdpinfo[0].port;
  mediaDetail.sdpinfo[0].port = 0;
  clientDetail.audioDetail.m_codec = mediaDetail.sdpinfo[0].codec;
  clientDetail.audioDetail.m_codecName = mediaDetail.sdpinfo[0].codecname;
  clientDetail.audioDetail.m_ptime = mediaDetail.sdpinfo[0].ptime;

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

#if 1 //TODO: these code block condition's are bit confusing, we need to review this later
  if((clientDetail.m_callDir == EN_CALL_DIR_IN) && \
      (clientDetail.m_mediaModeIn == EN_MEDIA_DTLS || clientDetail.m_mediaModeOut == EN_MEDIA_DTLS)) {
    //generateLocalSecureDetails(clientDetail);

    if (clientDetail.m_callType == EN_XGATE_CALL_TYPE_VIDEO) {
      clientDetail.videoDetail.m_mediaType = mediaDetail.sdpinfo[1].mediaType;
      clientDetail.videoDetail.m_clientIp = mediaDetail.sdpinfo[1].ip_addr;
      mediaDetail.sdpinfo[1].ip_addr = "";
      clientDetail.videoDetail.m_clientPort = mediaDetail.sdpinfo[1].port;
      mediaDetail.sdpinfo[1].port = 0;
      clientDetail.videoDetail.m_codec = mediaDetail.sdpinfo[1].codec;
      clientDetail.videoDetail.m_codecName = mediaDetail.sdpinfo[1].codecname;
      clientDetail.videoDetail.m_frameRate = mediaDetail.sdpinfo[1].framerate;
      clientDetail.videoDetail.m_imageattr_x = mediaDetail.sdpinfo[1].imageattr_x;
      clientDetail.videoDetail.m_imageattr_y = mediaDetail.sdpinfo[1].imageattr_y;

      //generate_local_video_secureDetails(clientDetail);
    }
  }

  //Note: if this condition satisfied means, we have already generated secure params 
  //and perserved them in opposite client structure. please swap those details
  if((clientDetail.m_callDir == EN_CALL_DIR_OUT) && \
      (clientDetail.m_mediaModeIn == EN_MEDIA_DTLS && clientDetail.m_mediaModeOut == EN_MEDIA_RTP)) {
    if(pHmpCall->m_clientCount > 0) {
      clientDetail.audioDetail.m_secureDetail = pHmpCall->m_clientList[0]->m_detail.audioDetail.m_secureDetail; 
    } else {
      XGLOG_ERROR( "set_media_details failed, origination client not yet allocated for mgresource_id:%s \
          and call_id:%s", (char*)ur_log_string(mediaDetail.mediaResource_id), (char*)ur_log_string(mediaDetail.call_id));
        return EN_XGATE_STATUS_ERROR;
    }
  }
#endif

  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpGstManager::update_media_details(ClientDetail &clientDetail, MgMediaDetail &mediaDetail)
{
  mediaDetail.sdpinfo[0].mediaType = clientDetail.audioDetail.m_mediaType;
  mediaDetail.sdpinfo[0].ip_addr = clientDetail.audioDetail.m_clientIp;
  mediaDetail.sdpinfo[0].port = clientDetail.audioDetail.m_clientPort;
  mediaDetail.sdpinfo[0].dial_ip = mediaDetail.sdpinfo[0].resp_ip = clientDetail.audioDetail.m_serverIp;
  mediaDetail.sdpinfo[0].resp_port = clientDetail.audioDetail.m_serverPort;
  mediaDetail.sdpinfo[0].ptime = clientDetail.audioDetail.m_ptime;

  XGLOG_INFO( " \n Created Message : resp_ip:%s resp_port:%d", (char*)ur_log_string(mediaDetail.sdpinfo[0].resp_ip), mediaDetail.sdpinfo[0].resp_port);

  if(clientDetail.m_mediaModeIn == EN_MEDIA_DTLS || clientDetail.m_mediaModeOut == EN_MEDIA_DTLS) {
    //update_media_secure_details(clientDetail, mediaDetail);

    if (clientDetail.m_callType == EN_XGATE_CALL_TYPE_VIDEO) {
      mediaDetail.sdpinfo[1].mediaType = clientDetail.videoDetail.m_mediaType;
      mediaDetail.sdpinfo[1].ip_addr = clientDetail.videoDetail.m_clientIp;
      mediaDetail.sdpinfo[1].port = clientDetail.videoDetail.m_clientPort;
      mediaDetail.sdpinfo[1].dial_ip = mediaDetail.sdpinfo[1].resp_ip = clientDetail.videoDetail.m_serverIp;
      mediaDetail.sdpinfo[1].resp_port = clientDetail.videoDetail.m_serverPort;
      mediaDetail.sdpinfo[1].framerate = clientDetail.videoDetail.m_frameRate;
      mediaDetail.sdpinfo[1].imageattr_x = clientDetail.videoDetail.m_imageattr_x;
      mediaDetail.sdpinfo[1].imageattr_y = clientDetail.videoDetail.m_imageattr_y;

      //update_video_secure_details(clientDetail, mediaDetail);
    }
  }
  return EN_XGATE_STATUS_SUCCESS;
}

#endif
