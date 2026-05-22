#include "xgconfig.h"
#ifdef XGATE_HAS_GSTREAMER 

//local include
#include "log.h"
#include "klog.h"

//self include
#include "xGateHmpGstManager.h"

#define THISMODULE "GstManag"

/**
 * Default Constructor
 */
//TODO: time configuration must be dynamic
xGateHmpGstManager::xGateHmpGstManager(const char* serverType)
{
  CLog::Detail(THISMODULE, "xGateHmpGstManager::xGateHmpGstManager(%s)", serverType);
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);
}

/**
 * Destructor
 */
xGateHmpGstManager::~xGateHmpGstManager (void)
{
  CLog::Detail(THISMODULE, "xGateHmpGstManager::~xGateHmpGstManager(%s) destructor called", m_serverType);
  m_running = false;
}//end destructor

/**
 * Initialize the xGateHmpGstManager
 */
xGateRetVal xGateHmpGstManager::init (void)
{

  CLog::Detail(THISMODULE, "xGateHmpGstManager::init(%s)", m_serverType);

  // Spawn the thread
  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1)
  {
    // Spawn Failure
    CLog::Detail(THISMODULE, "xGateHmpGstManager::init(%s)"\
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
  CLog::Detail(THISMODULE, "xGateHmpGstManager::run (starting gstreamer events processing loop)");

  xGateHmpGstManager* pGstManager = static_cast<xGateHmpGstManager*>(threadClass);     
  pGstManager->m_running = true; 

  //TODO: Yoga, below concepts needs to be build before loop
  //1. initialize gstreamer library
  if(pGstManager->initGstreamerLib() != EN_XGATE_STATUS_SUCCESS) {
    CLog::Error(THISMODULE, "xGateHmpGstManager::initGstreamerLib failed xGateHmpGstManager thread exit");
    return; 
  }

  //2. initialize Gstreamer MainEventLoop from this thread context
  GMainContext *ctx = g_main_context_ref_thread_default();
  GMainLoop *loop = g_main_loop_new (ctx, FALSE);
#if 0
  //3. construct different gstreamer elements, bin, pipeline and add them in channel pool 
  if(pGstManager->constructMediaElements() != EN_XGATE_STATUS_SUCCESS) {
    CLog::Error(THISMODULE, "xGateHmpGstManager::constructMediaElements failed xGateHmpGstManager thread exit");
    return; 
  }
#endif
  //    all the gstreamer related events handled from this thread context. 
  while (pGstManager->m_running)
  {
    //TODO: Yoga, below concepts needs to be build within loop
    g_main_context_iteration (ctx, FALSE); //TRUE - blocking
    usleep(2000); //2000 is 2 milliseconds
  }

  //deinitializing gstreamer library
  gst_deinit();
  CLog::Detail(THISMODULE, "xGateHmpGstManager::run reactor loop is ended");
}//end run

/**
 * Interface to stop the connector thread
 */
xGateRetVal xGateHmpGstManager::stopThread( void )
{
  CLog::Detail(THISMODULE, "xGateHmpGstManager::stopThread(%s) called", m_serverType);
  m_running = false;
  CLog::Detail(THISMODULE, "xGateHmpGstManager::stopThread(%s) Stopped xGateHmpGstManager", m_serverType);
  return EN_XGATE_STATUS_SUCCESS;
} 

xGateRetVal xGateHmpGstManager::initGstreamerLib()
{
  CLog::Detail(THISMODULE, "xGateHmpGstManager::initGstreamerLib(%s)", m_serverType);
  GError *err;
  if(!gst_init_check(0, NULL, &err)) {
    if(err) {
      CLog::Error(THISMODULE, "xGateHmpGstManager::initGstreamerLib failed to initialize GStreamer. Error: %d: %s", \
          err->code, err->message); 
      g_error_free(err);
    } else {
      CLog::Error(THISMODULE, "xGateHmpGstManager::initGstreamerLib failed to initialize GStreamer. Unknown Error");
    }
    return EN_XGATE_STATUS_ERROR;
  }

  guint major, minor, micro, nano;
  gst_version (&major, &minor, &micro, &nano);
  CLog::Detail(THISMODULE, "xGateHmpGstManager::initGstreamerLib successfully initialized Gstreamer version %d.%d.%d-%d", \
      major, minor, micro, nano);

  return EN_XGATE_STATUS_SUCCESS;
}

xGateHmpGstPipeline * xGateHmpGstManager::create_pipeline(HmpGstPipelineType pipelineType)
{
  string pipelineName("");
  HmpGstPipelineFactory *pPipelineFactory = new HmpGstPipelineFactory();

  switch(pipelineType) {
    case EN_PIPELINE_SINGLE_CALL:
      CLog::Detail(THISMODULE, "HmpGstPool::create_pipeline for PIPELINE_SINGLE_CALL type!");
      pipelineName = "Single_Call_Pipeline";
      break;

    case EN_PIPELINE_CONF_CALL:
      pipelineName = "Conf_Call_Pipeline";
      CLog::Detail(THISMODULE, "HmpGstPool::create_pipeline for PIPELINE_CONF_CALL type!");
      break;

    case EN_PIPELINE_ADD_CALL:
      pipelineName = "Add_Call_Pipeline";
      CLog::Detail(THISMODULE, "HmpGstPool::create_pipeline for PIPELINE_ADD_CALL type!");
      break;

    case EN_PIPELINE_UNKNOWN:
    default:
      CLog::Error(THISMODULE, "HmpGstPool::create_pipeline_pool failed for UNKNOWN pipeline type!");
      break;
  }

  m_pPipeline =  pPipelineFactory->pipeline_create(pipelineName, pipelineType);

  if(!m_pPipeline){
    CLog::Error(THISMODULE, "xGateHmpGstBin::create_pipeline Failed");
    return NULL;
  }

  CLog::Detail(THISMODULE, "xGateHmpGstBin::create_pipeline Successfull");

  return m_pPipeline;
}

xGateRetVal xGateHmpGstManager::create_udpbin_elements(xGateHmpGstPipeline *pHmpGstPipeline, ClientDetail &clientDetail)
{
  CLog::Detail(THISMODULE, "xGateHmpGstBin::create_udpbin_elements started");
  gboolean retVal = FALSE;
  Client *client = NULL;
  string channame;
  string binName("");
  HmpGstBinType binType;
  
  client = pHmpGstPipeline->insert_client_entry(clientDetail);

  snprintf(channame.c_str(), 50, "%d_%s", clientDetail.m_legid, "Voice_Pipeline");
  client->m_voiceChannel = new HmpGstVoiceChannel(channame);
  client->m_voiceChannel->create_voice_channel();

  client->m_voiceChannel->m_binFactory = new HmpGstBinFactory();

  HmpGstCodecType codecType = client->m_detail.m_codec;
  switch(codecType) {
    case EN_RTP_CODEC:
      binName = "RTP_Bin";
      binType = EN_BIN_RTP;
      break;     

    case EN_PCMA_CODEC:
      binName = "G711_Bin";
      binType = EN_BIN_G711;
      break;

    case EN_G729_CODEC:
      binName = "G729_Bin";
      binType = EN_BIN_G729;
      break;

    case EN_OPUS_CODEC:
      binName = "OPUS_Bin";
      binType = EN_BIN_OPUS;
      break;     
    case EN_UNKNOWN_CODEC:
    default:
      CLog::Error(THISMODULE, "xGateHmpGstBin::create_udpbin_elements EN_UNKNOWN_CODEC!");
      return EN_XGATE_STATUS_ERROR;
  }

  client->m_voiceChannel->m_voiceBin = client->m_voiceChannel->m_binFactory->bin_create(binName, binType, client->m_detail);
  if(!client->m_voiceChannel->m_voiceBin) {
    CLog::Error(THISMODULE, "xGateHmpGstBin::create_udpbin_elements failed for UNKNOWN bin type!");
    return EN_XGATE_STATUS_ERROR;
  }
  
  client->m_voiceChannel->m_termBin = new HmpUdpTermBin(binName); 
  client->m_voiceChannel->m_termBin->create_termBin();

  snprintf(channame.c_str(), 50, "%d_%s", clientDetail.m_legid, "Play_Pipeline");
  client->m_playChannel = new HmpGstPlayChannel(channame);
  client->m_playChannel->create();

  snprintf(channame.c_str(), 50, "%d_%s", clientDetail.m_legid, "Play_Bin");
  client->m_playChannel->m_hmpPlayFileBin = new HmpPlayFileBin(channame);
  client->m_playChannel->m_hmpPlayFileBin->create_bin();

  client->m_playChannel->m_pbTermBin = new HmpUdpTermBin(channame);
  client->m_playChannel->m_pbTermBin->create_termBin();

  client->m_voiceChannel->activate_voice_channel(client);
  client->m_playChannel->setting_playfile_channel(client);

  CLog::Detail(THISMODULE, "xGateHmpGstBin::create_udpbin_elements Successfull");
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpGstManager::set_client_details(ClientDetail &clientDetail, MgMediaDetail &mediaDetail)
{
  int isDTLS = 0; 
  bzero(ice_ufrag,sizeof(ice_ufrag));
  bzero(icepwd,sizeof(icepwd));
  bzero(fingerprt,sizeof(fingerprt));

  clientDetail.m_callId = mediaDetail.call_id;
/*
  clientDetail.m_ip = mediaDetail.ip_addr;
  mediaDetail.ip_addr = "";
  clientDetail.m_port = mediaDetail.port;
  mediaDetail.port = 0;
  clientDetail.m_relayIpaddr =  mediaDetail.relayip_addr;
  clientDetail.m_relayPort = mediaDetail.relay_port;
  clientDetail.m_reflexIpaddr =  mediaDetail.reflexip_addr;
  clientDetail.m_reflexPort = mediaDetail.reflex_port;
  clientDetail.m_codec = mediaDetail.codec;
  clientDetail.m_pTime = mediaDetail.ptime;
*/
  clientDetail.m_callDir = mediaDetail.call_dir;
  clientDetail.m_sigType = mediaDetail.sig_type;

  if(mediaDetail.media_proto.compare("DTLS")== 0) {
    clientDetail.m_mediaMode = EN_MEDIA_DTLS;
    isDTLS = 1;
    if(mediaDetail.out_proto.compare("DTLS") == 0) {
      clientDetail.m_outProto = EN_MEDIA_DTLS;
    }
  } else if(mediaDetail.media_proto.compare("SRTP")== 0) {
    clientDetail.m_mediaMode = EN_MEDIA_SRTP;
  } else if(mediaDetail.media_proto.compare("RTP")== 0) {
    clientDetail.m_mediaMode = EN_MEDIA_RTP;
    if(mediaDetail.out_proto.compare("DTLS") == 0) {
      clientDetail.m_outProto = EN_MEDIA_DTLS;
      isDTLS = 1;
    }
  } else clientDetail.m_mediaMode = EN_MEDIA_UNKNOWN;

  if(isDTLS)  {
    SetDTLSParams();
    // Creating ssl certificate pem file and fingerprint
    generateSSLCertFile();
    clientDetail.m_pemFile = sslCertFileName;
/*
    clientDetail.m_fingerPrint =  mediaDetail.fingerprint;
    mediaDetail.fingerprint = "";
    mediaDetail.fingerprint = fingerprt;
    //received remote ufrag
    clientDetail.m_remIceUfrag =  mediaDetail.ice_ufrag;
    mediaDetail.ice_ufrag = "";
    //Creating local ufrag and setting local for Client
    generateUsername();
    mediaDetail.ice_ufrag = ice_ufrag;
    //Setting local ufrag for MG 
    clientDetail.m_locIceUfrag ="";
    clientDetail.m_locIceUfrag = ice_ufrag;
    //creating password
    generatePassword();
    clientDetail.m_icePwd = icepwd;
    mediaDetail.ice_pwd = "";
    mediaDetail.ice_pwd = icepwd;

    generateSSRC();
    clientDetail.m_ssrc = ssrc;
    mediaDetail.ssrc = ssrc;

    generateCNANE();
    clientDetail.m_cname = cname;
    mediaDetail.cname = cname;

    generateMSLABEL();
    clientDetail.m_mslabel = mslabel;
    mediaDetail.mslabel = mslabel;

    generateLABEL();
    clientDetail.m_label = label;
    mediaDetail.label = label;

    CLog::Detail(THISMODULE, "Setting Client details Generate fingerprint %s ice_ufrag %s ice_pwd %s ", mediaDetail.fingerprint.c_str(), mediaDetail.ice_ufrag.c_str(), mediaDetail.ice_pwd.c_str());
    CLog::Detail(THISMODULE, "Generated SSRC %s CNANE %s MSLABEL %s LABEL %s ", mediaDetail.ssrc.c_str(), mediaDetail.cname.c_str(), mediaDetail.mslabel.c_str(), mediaDetail.label.c_str());
*/
  } 
}

void xGateHmpGstManager::setSecurityParams(Client *clientOrg, ClientDetail &clientDetail)
{
  clientDetail.m_pemFile ="";
  clientDetail.m_pemFile = clientOrg->m_detail.m_pemFile;
  clientDetail.m_locIceUfrag ="";
  clientDetail.m_locIceUfrag = clientOrg->m_detail.m_locIceUfrag;
  clientDetail.m_icePwd ="";
  clientDetail.m_icePwd = clientOrg->m_detail.m_icePwd;
}

xGateRetVal xGateHmpGstManager::remove_rtp_bins(xGateHmpGstPipeline *pHmpGstPipeline, Client *clientOrg)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  //clientOrg->m_hmpBin->sendBin_remove();
  //clientOrg->m_hmpBin->recvBin_remove();

  return EN_XGATE_STATUS_SUCCESS;
}


xGateRetVal xGateHmpGstManager::handle_dtmf_request(MgMediaDetail &mediaDetail)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  Client *client = NULL;
  xGatePbState pbState = EN_XGATE_PB_NULL;
  unsigned int callLegId = 0;

  xGateHmpGstPipeline *pHmpGstPipeline = NULL;
  HmpGstVoiceChannel  *pHmpVoiceChannel = NULL;

  pHmpGstPipeline = find_channel_entry_in_map(mediaDetail.call_id);
  if(!pHmpGstPipeline) {
    CLog::Detail(THISMODULE, "handle_dtmf_request! no call entry for call_id: %s", mediaDetail.call_id.c_str());
    return EN_XGATE_STATUS_ERROR;
  }

  callLegId = (mediaDetail.leg_id > 0) ? --mediaDetail.leg_id : mediaDetail.leg_id;
  client = pHmpGstPipeline->m_clientList[callLegId];
  if(!client) {
    CLog::Detail(THISMODULE, "xGateHmpGstManager::handle_dtmf_request failed! client not available for leg_id: %d", mediaDetail.leg_id);
    return EN_XGATE_STATUS_ERROR;
  }

  pHmpVoiceChannel = client->m_voiceChannel;
  if(!pHmpVoiceChannel) {
    CLog::Error(THISMODULE, "xGateHmpGstManager::handle_dtmf_request failed! PlayBack Pipeline is NULL for call_id: %s", mediaDetail.call_id.c_str());
    return EN_XGATE_STATUS_ERROR;
  }

  CLog::Detail(THISMODULE, "xGateHmpGstManager::handle_dtmf_request! call entry for call_id: %s , pbState %d", mediaDetail.call_id.c_str(), pbState);
 
  pbState = mediaDetail.pb_state;
  switch(pbState) {
    case EN_XGATE_PB_GETDIGIT:
        CLog::Detail(THISMODULE, "xGateHmpGstManager::handle_dtmf_request: collecting DTMF for call_id: %s", mediaDetail.call_id.c_str());
        pHmpVoiceChannel->get_dtmf_digit(client);
      break;

    case EN_XGATE_PB_SENDDIGIT:
        CLog::Detail(THISMODULE, "xGateHmpGstManager::handle_dtmf_request: sending collected DTMF for call_id: %s", mediaDetail.call_id.c_str());
        pHmpVoiceChannel->send_dtmf_digit(client);
      break;

    case EN_XGATE_PB_NULL:
    default:
        CLog::Error(THISMODULE, "xGateHmpGstManager::handle_dtmf_request failed! unsupported DTMF state for call_id: %s", mediaDetail.call_id.c_str());
      return EN_XGATE_STATUS_ERROR;
  }
  return EN_XGATE_STATUS_SUCCESS;
}


xGateRetVal xGateHmpGstManager::allocate_orig_channel(MgMediaDetail &mediaDetail)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  ClientDetail clientDetail;
  Client *clientOrginator = NULL;
  xGateHmpGstPipeline *pHmpGstPipeline = NULL;
  gboolean isEntryExist = TRUE;
/*
  CLog::Detail(THISMODULE, "xGateHmpGstManager::allocate_orig_channel for call_id: %s, client_ip: %s client_port: %d codec: %d, sig_type: %d, call_dir %d", \
      mediaDetail.call_id.c_str(), mediaDetail.ip_addr.c_str(), mediaDetail.port, \
      mediaDetail.codec, mediaDetail.sig_type, mediaDetail.call_dir);

  set_client_details(clientDetail, mediaDetail);

  if(clientDetail.m_callId.empty() || clientDetail.m_ip.empty() || clientDetail.m_port <= 0) {
    CLog::Error(THISMODULE, "xGateHmpGstManager::allocate_orig_channel failed due to client media details");
    return retVal;
  }

  //1. Check whether the Channel created and its entry available in Map using call_id
  pHmpGstPipeline = find_channel_entry_in_map(mediaDetail.call_id);
  if(!pHmpGstPipeline) { //first time adding client in map for this call_id
    pHmpGstPipeline = create_pipeline(EN_PIPELINE_SINGLE_CALL);
    isEntryExist = FALSE;
  }

  if(pHmpGstPipeline) {
    clientDetail.m_legid = 0;
    clientDetail.m_serverPort = HOSTGST_RTP_GET_PORT();
    clientDetail.m_serverIp =  xGateUtil::getLocalIpAddr();//xGateUtil::GetLocalIPAddress();
    CLog::Detail(THISMODULE, "xGateHmpGstManager::allocate_orig_channel for call_id: %s, server_ip: %s server_port: %d", \
        mediaDetail.call_id.c_str(), clientDetail.m_serverIp.c_str(), clientDetail.m_serverPort);

    retVal = create_udpbin_elements(pHmpGstPipeline, clientDetail);
    if(retVal == EN_XGATE_STATUS_SUCCESS) {
      clientOrginator = pHmpGstPipeline->m_clientList[0];
      mediaDetail.ip_addr = xGateUtil::getLocalIpAddr();
      clientOrginator->m_terPort = HOSTGST_RTP_GET_PORT();   
      mediaDetail.port = clientOrginator->m_terPort;
      mediaDetail.ptime = 20;
    }
    CLog::Detail(THISMODULE, "xGateHmpGstManager::allocate_orig_channel, allocated media address is %s:%d", \
       mediaDetail.ip_addr.c_str(), mediaDetail.port);
  }

  //on success add this detail into map(callid vs pipeline object)
  if(!isEntryExist) {
    add_channel_entry_in_map(clientDetail.m_callId, pHmpGstPipeline);
  }

  if(!mediaDetail.ip_addr.empty() && mediaDetail.port > 0) {
    CLog::Detail(THISMODULE, "xGateHmpGstManager::allocate_orig_channel allocated Leg-B media channel ip: %s, port: %d", \
        mediaDetail.ip_addr.c_str(), mediaDetail.port);
    retVal = EN_XGATE_STATUS_SUCCESS;
  } else {
    CLog::Error(THISMODULE, "xGateHmpGstManager::allocate_orig_channel failed due to media port allocation problem");
    retVal = EN_XGATE_STATUS_ERROR;
  }
*/
  return retVal;
}

xGateRetVal xGateHmpGstManager::handle_18X_response(MgMediaDetail &mediaDetail)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  Client *clientOrg = NULL;
  xGateHmpGstPipeline *pHmpGstPipeline = NULL;

  pHmpGstPipeline = find_channel_entry_in_map(mediaDetail.call_id);
  if(!pHmpGstPipeline) {
    CLog::Detail(THISMODULE, "xGateHmpGstManager::handle_18X_response failed no call entry for call_id: %s", mediaDetail.call_id.c_str());
    return EN_XGATE_STATUS_ERROR;
  }

/*
  if(pHmpGstPipeline) {
    clientOrg = pHmpGstPipeline->m_clientList[0];
    mediaDetail.ip_addr = "";
    mediaDetail.ip_addr =  clientOrg->m_detail.m_serverIp;
    mediaDetail.port = clientOrg->m_detail.m_serverPort;
    mediaDetail.codec = clientOrg->m_detail.m_codec;
    mediaDetail.ptime = clientOrg->m_detail.m_pTime;   // ptime received from Leg-A through SIP-INVITE msg
    if(clientOrg->m_detail.m_mediaMode == EN_MEDIA_DTLS) {
       mediaDetail.relayip_addr = clientOrg->m_detail.m_relayIpaddr;
       mediaDetail.relay_port = clientOrg->m_detail.m_relayPort;
       mediaDetail.reflexip_addr = clientOrg->m_detail.m_reflexIpaddr;
       mediaDetail.reflex_port = clientOrg->m_detail.m_reflexPort;
       mediaDetail.fingerprint = clientOrg->m_detail.m_fingerPrint;
       mediaDetail.ice_ufrag = clientOrg->m_detail.m_locIceUfrag;
       mediaDetail.ice_pwd = clientOrg->m_detail.m_icePwd;
       mediaDetail.ssrc = clientOrg->m_detail.m_ssrc;
       mediaDetail.cname = clientOrg->m_detail.m_cname;
       mediaDetail.mslabel = clientOrg->m_detail.m_mslabel;
       mediaDetail.label = clientOrg->m_detail.m_label;
    }
  }

  if(!mediaDetail.ip_addr.empty() && mediaDetail.port > 0) {
    CLog::Detail(THISMODULE, "xGateHmpGstManager::handle_18X_response allocated media channel ip: %s, port: %d", \
        mediaDetail.ip_addr.c_str(), mediaDetail.port);
    retVal = EN_XGATE_STATUS_SUCCESS;
  } else {
    CLog::Error(THISMODULE, "xGateHmpGstManager::handle_18X_response failed due to media port allocation problem");
    retVal = EN_XGATE_STATUS_ERROR;
  }

*/
  return retVal;
}

xGateRetVal xGateHmpGstManager::handle_play_back(MgMediaDetail &mediaDetail)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  //string playFile("");
  bool autorepeat = false;
  Client *client = NULL;
  Client *dclient = NULL;
  unsigned int callLegId = 0;
  xGatePbState pbState = EN_XGATE_PB_NULL;
  xGateHmpGstPipeline *pHmpGstPipeline = NULL;
  HmpGstVoiceChannel *pHmpVoiceChannel = NULL;
  HmpGstPlayChannel *pHmpPlayChannel = NULL;

  pHmpGstPipeline = find_channel_entry_in_map(mediaDetail.call_id);
  if(!pHmpGstPipeline) {
    CLog::Error(THISMODULE, "xGateHmpGstManager::handle_play_back failed! no call entry for call_id: %s", mediaDetail.call_id.c_str());
    return EN_XGATE_STATUS_ERROR;
  }
  
  callLegId = (mediaDetail.leg_id > 0) ? --mediaDetail.leg_id : mediaDetail.leg_id;
  client = pHmpGstPipeline->m_clientList[callLegId];
  if(!client) {
    CLog::Detail(THISMODULE, "xGateHmpGstManager::handle_play_back failed! client not available for leg_id: %d", mediaDetail.leg_id);
    return EN_XGATE_STATUS_ERROR;
  }

  pHmpPlayChannel = client->m_playChannel;
  pHmpVoiceChannel = client->m_voiceChannel;
  if(!pHmpPlayChannel || !pHmpVoiceChannel) {
    CLog::Error(THISMODULE, "xGateHmpGstManager::handle_play_back failed! PlayBack Pipeline is NULL for call_id: %s", mediaDetail.call_id.c_str());
    return EN_XGATE_STATUS_ERROR;
  }
  

  pbState = mediaDetail.pb_state;
  switch(pbState) {
    case EN_XGATE_PB_PLAY:
    {
      CLog::Error(THISMODULE, "xGateHmpGstManager::handle_play_back EN_XGATE_PB_PLAY for call_id: %s", mediaDetail.call_id.c_str());
//      pHmpPlayChannel->activate_fileplay_channel(client, mediaDetail.play_file, FALSE);
      break;
    }
    case EN_XGATE_PB_STOP:
    {
      CLog::Error(THISMODULE, "xGateHmpGstManager::handle_play_back EN_XGATE_PB_STOP for call_id: %s", mediaDetail.call_id.c_str());
 //     pHmpPlayChannel->deactivate_fileplay_channel(client);
      break;
    }
    case EN_XGATE_PB_REPEAT:
    {
      CLog::Error(THISMODULE, "xGateHmpGstManager::handle_play_back EN_XGATE_PB_REPEAT for call_id: %s", mediaDetail.call_id.c_str());
//      pHmpPlayChannel->activate_fileplay_channel(client, mediaDetail.play_file, TRUE);
      break;
    }
    case EN_XGATE_PB_HOLD:
    {
      CLog::Error(THISMODULE, "xGateHmpGstManager::handle_play_back EN_XGATE_PB_HOLD for call_id: %s", mediaDetail.call_id.c_str());
      //pHmpGstPipeline->pipeline_stop_media();
//      pHmpPlayChannel->activate_fileplay_channel(client, mediaDetail.play_file, TRUE);
      break;
    } 
    case EN_XGATE_PB_RESUME:
    {
      CLog::Error(THISMODULE, "xGateHmpGstManager::handle_play_back EN_XGATE_PB_RESUME for call_id: %s", mediaDetail.call_id.c_str());
      pHmpPlayChannel->deactivate_fileplay_channel(client);
      //pHmpGstPipeline->pipeline_start_media();
      break;
    } 
    case EN_XGATE_PB_NULL:
    default:
      {
        CLog::Error(THISMODULE, "xGateHmpGstManager::handle_play_back failed! unsupported playback state for call_id: %s", mediaDetail.call_id.c_str());
        return EN_XGATE_STATUS_ERROR;
      }
  }
  return EN_XGATE_STATUS_SUCCESS;

}

  xGateRetVal xGateHmpGstManager::allocate_term_channel(MgMediaDetail &mediaDetail)
  {
    xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
    ClientDetail clientDetail;
    Client *clientOrginator = NULL;
    Client *clientTerminator = NULL;
    xGateHmpGstPipeline *pHmpGstPipeline = NULL;

//    CLog::Detail(THISMODULE, "xGateHmpGstManager::allocate_term_channel for call_id: %s, client_ip: %s client_port: %d codec: %d, sig_type: %d, call_dir %d", \
//        mediaDetail.call_id.c_str(), mediaDetail.ip_addr.c_str(), mediaDetail.port, \
//        mediaDetail.codec, mediaDetail.sig_type, mediaDetail.call_dir);

    set_client_details(clientDetail, mediaDetail);

    if(clientDetail.m_callId.empty() || clientDetail.m_ip.empty() || clientDetail.m_port <= 0) {
      CLog::Error(THISMODULE, "xGateHmpGstManager::allocate_term_channel failed due to client media details");
      return EN_XGATE_STATUS_ERROR;
    }

    pHmpGstPipeline = find_channel_entry_in_map(mediaDetail.call_id);
    if(!pHmpGstPipeline) { 
      CLog::Detail(THISMODULE, "xGateHmpGstManager::allocate_term_channel failed no call entry for call_id: %s", mediaDetail.call_id.c_str());
      return EN_XGATE_STATUS_ERROR;
    }

    if(pHmpGstPipeline) {
      clientOrginator = pHmpGstPipeline->m_clientList[0];

      if(clientOrginator->m_detail.m_serverPort > 0 ) {
//        mediaDetail.port = clientOrginator->m_detail.m_serverPort;
      } else {
//        mediaDetail.port = HOSTGST_RTP_GET_PORT();
      }

      clientDetail.m_serverPort = clientOrginator->m_terPort;
      clientDetail.m_serverIp = xGateUtil::getLocalIpAddr();
      clientDetail.m_legid = 1;

      if(clientOrginator->m_detail.m_outProto == EN_MEDIA_DTLS) {
        setSecurityParams(clientOrginator, clientDetail);
      }
      
      retVal = create_udpbin_elements(pHmpGstPipeline, clientDetail);
      //first update client detail into client list
      if(retVal == EN_XGATE_STATUS_SUCCESS) {      
        clientTerminator = pHmpGstPipeline->m_clientList[1];
//        mediaDetail.ip_addr = clientDetail.m_serverIp;
//        mediaDetail.ptime = clientOrginator->m_detail.m_pTime;
      }
    }

//    if(!mediaDetail.ip_addr.empty() && mediaDetail.port > 0) {
//      CLog::Detail(THISMODULE, "xGateHmpGstManager::allocate_term_channel allocated media channel ip: %s, port: %d", \
//          mediaDetail.ip_addr.c_str(), mediaDetail.port);
//      retVal = EN_XGATE_STATUS_SUCCESS;
//    } else {
//      CLog::Error(THISMODULE, "xGateHmpGstManager::allocate_term_channel failed due to media port allocation problem");
//      retVal = EN_XGATE_STATUS_ERROR;
//    }

    return retVal;
    }

    xGateRetVal xGateHmpGstManager::connect_media_channel(MgMediaDetail &mediaDetail)
    {
      Client *clientOrg = NULL;
      Client *clientTer = NULL;
      xGateHmpGstPipeline *pHmpGstPipeline = NULL;
      int listSize = 0;

      CLog::Detail(THISMODULE, "xGateHmpGstManager::connect_media_channel connecting simple channel for call_id: %s", mediaDetail.call_id.c_str());

      pHmpGstPipeline = find_channel_entry_in_map(mediaDetail.call_id);
      if(!pHmpGstPipeline) {
        CLog::Detail(THISMODULE, "xGateHmpGstManager::connect_media_channel failed no call entry for call_id: %s", mediaDetail.call_id.c_str());
        return EN_XGATE_STATUS_ERROR;
      }
      listSize = pHmpGstPipeline->m_clientList.size();

      if(listSize == 2) {
        clientOrg  = pHmpGstPipeline->m_clientList[0];
        clientTer  = pHmpGstPipeline->m_clientList[1];

        if(!pHmpGstPipeline->pipeline_connect_clients(clientOrg, clientTer)) {
          CLog::Error(THISMODULE, "xGateHmpGstManager::connect_media_channel, pipeline_connect_clients failed for call_id: %s", \
              mediaDetail.call_id.c_str());
          return EN_XGATE_STATUS_ERROR;
        }
      } else { 
        CLog::Error(THISMODULE, "xGateHmpGstManager::connect_media_channel, pipeline_connect_clients failed for call_id: %s", \
            mediaDetail.call_id.c_str());
        return EN_XGATE_STATUS_ERROR;
      }

      CLog::Detail(THISMODULE, "xGateHmpGstManager::connect_media_channel connected simple channel for call_id: %s", mediaDetail.call_id.c_str());
      return EN_XGATE_STATUS_SUCCESS;
    }

    xGateRetVal xGateHmpGstManager::release_channel(MgMediaDetail &mediaDetail)
    {
      xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
      Client *clientOrg = NULL;
      Client *clientTer = NULL;
      xGateHmpGstPipeline *pHmpGstPipeline = NULL;
      int listSize = 0;

      CLog::Detail(THISMODULE, "trying to release simple channel for call_id: %s", mediaDetail.call_id.c_str());

      //1. check acquiredChannelMap using call_id
      pHmpGstPipeline = find_channel_entry_in_map(mediaDetail.call_id); 

      if(!pHmpGstPipeline) {
        CLog::Detail(THISMODULE, "release_channel failed no call entry for call_id: %s", mediaDetail.call_id.c_str());
        return EN_XGATE_STATUS_ERROR;
      }

      //2. Checking both Legs are created
      listSize = pHmpGstPipeline->m_clientList.size();
      if(listSize == 0) {
        erase_channel_entry_in_map(mediaDetail.call_id);
        CLog::Detail(THISMODULE, "release_channel failed no client details for call_id: %s", mediaDetail.call_id.c_str());
        return EN_XGATE_STATUS_ERROR;
      }

      //2.1 If both Legs are present
      if(listSize == 2) {
        clientOrg  = pHmpGstPipeline->m_clientList[0];   
        clientTer  = pHmpGstPipeline->m_clientList[1];

        if (clientOrg->m_detail.m_mediaMode == EN_MEDIA_DTLS){
          removePemFile(clientOrg->m_detail.m_pemFile);
        }
        if (clientTer->m_detail.m_mediaMode == EN_MEDIA_DTLS){
          removePemFile(clientTer->m_detail.m_pemFile);
        }

        if (clientOrg && clientTer) {
          retVal = pHmpGstPipeline->disconnect_simple_client(clientOrg, clientTer);
          if(clientOrg->m_voiceChannel){
             clientOrg->m_voiceChannel->deactivate_voice_channel(clientOrg);
             if(clientOrg->m_voiceChannel->m_voiceBin) delete clientOrg->m_voiceChannel->m_voiceBin;
             if(clientOrg->m_voiceChannel->m_termBin) delete clientOrg->m_voiceChannel->m_termBin;
             if(clientOrg->m_voiceChannel->m_binFactory) delete clientOrg->m_voiceChannel->m_binFactory;
             if(clientOrg->m_voiceChannel) delete clientOrg->m_voiceChannel;
          }
          if(clientOrg->m_playChannel) {
             clientOrg->m_playChannel->deactivate_fileplay_channel(clientOrg);
             if(clientOrg->m_playChannel->m_pbTermBin) delete clientOrg->m_playChannel->m_pbTermBin;
             if(clientOrg->m_playChannel) delete clientOrg->m_playChannel;
          }
          if(clientTer->m_voiceChannel) {
             clientTer->m_voiceChannel->deactivate_voice_channel(clientTer);
             if(clientTer->m_voiceChannel->m_voiceBin) delete clientTer->m_voiceChannel->m_voiceBin;
             if(clientTer->m_voiceChannel->m_termBin) delete clientTer->m_voiceChannel->m_termBin;
             if(clientTer->m_voiceChannel->m_binFactory) delete clientTer->m_voiceChannel->m_binFactory;
             if(clientTer->m_voiceChannel) delete clientTer->m_voiceChannel;
          }
          if(clientTer->m_playChannel) {
             clientTer->m_playChannel->deactivate_fileplay_channel(clientTer);
             if(clientTer->m_playChannel->m_pbTermBin) delete clientTer->m_playChannel->m_pbTermBin;
             if(clientTer->m_playChannel) delete clientTer->m_playChannel;
          }
          pHmpGstPipeline->remove_client_entries();
        } else return EN_XGATE_STATUS_ERROR;

        //2.2 If Leg-A or Leg-B is present
      } if (listSize == 1) {
        clientOrg  = pHmpGstPipeline->m_clientList[0];
        if (clientOrg) {
          if (clientOrg->m_detail.m_mediaMode == EN_MEDIA_DTLS){
            removePemFile(clientOrg->m_detail.m_pemFile);
          }
         cout << endl << "trying to release simple channel for clientOrg" << endl;
         if(clientOrg->m_voiceChannel){
             clientOrg->m_voiceChannel->deactivate_voice_channel(clientOrg);
             if(clientOrg->m_voiceChannel->m_voiceBin) delete clientOrg->m_voiceChannel->m_voiceBin;
             if(clientOrg->m_voiceChannel->m_termBin) delete clientOrg->m_voiceChannel->m_termBin;
             if(clientOrg->m_voiceChannel->m_binFactory) delete clientOrg->m_voiceChannel->m_binFactory;
             if(clientOrg->m_voiceChannel) delete clientOrg->m_voiceChannel;
         }
         if(clientOrg->m_playChannel) {
            clientOrg->m_playChannel->deactivate_fileplay_channel(clientOrg);
            if(clientOrg->m_playChannel->m_pbTermBin) delete clientOrg->m_playChannel->m_pbTermBin;
            if(clientOrg->m_playChannel) delete clientOrg->m_playChannel;
         }
          pHmpGstPipeline->remove_client_entry(clientOrg);
        } else return EN_XGATE_STATUS_ERROR;
        cout << endl << "trying to release simple channel for clientOrg" << endl;
      }

      //delete HmpGstPipeline object to call its destructor 
      delete pHmpGstPipeline;

      //3. remove map entry
      if (retVal == false) {  
        CLog::Detail(THISMODULE, "release_channel (failed) to remove bins for call_id: %s", mediaDetail.call_id.c_str());
        erase_channel_entry_in_map(mediaDetail.call_id);     
        return EN_XGATE_STATUS_ERROR;
      }

      erase_channel_entry_in_map(mediaDetail.call_id);
      CLog::Detail(THISMODULE, "release_channel (successfull) to remove resource for call_id: %s", mediaDetail.call_id.c_str());
      return EN_XGATE_STATUS_SUCCESS; 
    }

    xGateHmpGstPipeline * xGateHmpGstManager::find_channel_entry_in_map(string callId)
    {
      xGateHmpGstPipeline *pHmpGstPipeline = NULL;
      if(m_acquiredChannelMap.find(callId.c_str(), pHmpGstPipeline) != -1) {
        CLog::Detail(THISMODULE, "find_channel_entry_in_map success for call_id: %s", callId.c_str());
      } else {
        //CLog::Error(THISMODULE, "find_channel_entry_in_map failed for call_id: %s", callId.c_str());
      }
      return pHmpGstPipeline;
    }

    xGateRetVal xGateHmpGstManager::add_channel_entry_in_map(string callId, xGateHmpGstPipeline* hmpGstPipeline)
    {
      xGateRetVal retVal = EN_XGATE_STATUS_ERROR; 
      int status = -1;

      status = m_acquiredChannelMap.bind(callId.c_str(), hmpGstPipeline);
      if(status == 0) {
        CLog::Detail(THISMODULE, "add_channel_entry_in_map success for call_id: %s", callId.c_str());
        retVal = EN_XGATE_STATUS_SUCCESS;
      } else if(status == 1) {
        CLog::Detail(THISMODULE, "add_channel_entry_in_map attempt is made to bind an existing entry for call_id: %s", callId.c_str());
      } else if(status == -1) {
        CLog::Error(THISMODULE, "add_channel_entry_in_map failed for call_id: %s", callId.c_str()); 
      }
      return retVal;
    }

    xGateRetVal xGateHmpGstManager::erase_channel_entry_in_map(string callId)
    {
      CLog::Detail(THISMODULE, "erase_channel_entry_in_map for call_id: %s", callId.c_str()); 
      m_acquiredChannelMap.unbind(callId.c_str());
    }
void xGateHmpGstManager::generateSSRC()
{
   char *pout;
   int i;
#define SSRC_TEMPLATE "0123456789"
   pout = ssrc;
   for(i = 0; i < SSRC_LENGTH ; i++)
   {
     pout[i] = SSRC_TEMPLATE[rand()%strlen(SSRC_TEMPLATE)];
   }
   pout[i]='\0';
   CLog::Info(THISMODULE,"GENERATED SSRC %s\n",ssrc);
}

void xGateHmpGstManager::generateCNANE()
{
   char *pout;
   int i;
#define CNAME_TEMPLATE "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
   pout = cname;
   for(i = 0; i < CNAME_LENGTH ; i++)
   {
     pout[i] = CNAME_TEMPLATE[rand()%strlen(CNAME_TEMPLATE)];
   }
   pout[i]='\0';
   CLog::Info(THISMODULE,"GENERATED CNAME %s\n",cname);
}

void xGateHmpGstManager::generateMSLABEL()
{
   char *pout;
   int i;
#define MSLABEL_TEMPLATE "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
   pout = mslabel;
   for(i = 0; i < MSLABEL_LENGTH ; i++)
   {
     pout[i] = MSLABEL_TEMPLATE[rand()%strlen(MSLABEL_TEMPLATE)];
   }
   pout[i]='\0';
   CLog::Info(THISMODULE,"GENERATED MSLABEL %s\n",mslabel);
}

void xGateHmpGstManager::generateLABEL()
{
   int len;
   char labelStr[12];

   label_str(labelStr,8);
   sprintf(label, "%s", labelStr);

   label_str(labelStr,4);
   sprintf(label, "%s-%s", label, labelStr);

   label_str(labelStr,4);
   sprintf(label, "%s-%s", label, labelStr);

   label_str(labelStr,4);
   sprintf(label, "%s-%s", label, labelStr);

   label_str(labelStr,12);
   sprintf(label, "%s-%s", label, labelStr);

   CLog::Info(THISMODULE,"GENERATED LABEL %s\n",label);
}

void xGateHmpGstManager::label_str(char* labelstr, int label_len)
{
   char *pout;
   int i;
#define LABEL_TEMPLATE "0123456789abcdefghijklmnopqrstuvwxyz"
   pout = labelstr;
   for(i = 0; i < label_len; i++)
   {
     pout[i] = LABEL_TEMPLATE[rand()%strlen(LABEL_TEMPLATE)];
   }
   pout[i]='\0';
   CLog::Info(THISMODULE,"GENERATED MSLABEL %s\n",labelstr);
}

    void xGateHmpGstManager::generatePassword()
    {
      char *pout;
      int i;
#define PASSWORD_TEMPLATE "0123456789!@#$%^&*ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
      if( !strcmp(icepwd, TEMPLATE_STRING) ) {
        pout = icepwd;
        for(i = 0; i < PASSWORD_LENGTH ; i++)
        {
          pout[i] = PASSWORD_TEMPLATE[rand()%strlen(PASSWORD_TEMPLATE)];
        }
        pout[i]='\0';
        CLog::Info(THISMODULE,"GENERATED Password %s\n",icepwd);
      }
    }

    void xGateHmpGstManager::generateUsername()
    {
      int i;
      char *pout;
#define USERNAME_TEMPLATE "0123456789/ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

      if( !strcmp(ice_ufrag, TEMPLATE_STRING) )
      {
        pout = ice_ufrag;
        for(int i = 0; i < USERNAME_LENGTH; i++)
        {
          pout[i] = USERNAME_TEMPLATE[rand()%strlen(USERNAME_TEMPLATE)];
        }
        pout[i]='\0';
        CLog::Info(THISMODULE,"GENERATED UserName %s\n",ice_ufrag);
      }
    }

    int xGateHmpGstManager::SetDTLSParams()
    {
      CLog::Info(THISMODULE,"SetDTLSParams \n");
      strcpy(ice_candi,"candidate");
      if(!strlen(icepwd) && strcmp(icepwd,TEMPLATE_STRING)) {
        strcpy(icepwd,TEMPLATE_STRING);
        strcpy(ice_ufrag,TEMPLATE_STRING);
        strcpy(fingerprt,TEMPLATE_STRING);
        CLog::Info(THISMODULE,"SetDTLSParams: set to standardStrings\n");
      }
    }


    bool xGateHmpGstManager::generateSSLCertFile()
    {
      EVP_PKEY * pkey;
      X509 * x509;
      if(!strcmp(fingerprt,TEMPLATE_STRING))
      {
        /* Allocate memory for the EVP_PKEY structure. */
        EVP_PKEY * pkey = EVP_PKEY_new();
        if(!pkey)
        {
          std::cerr << "Unable to create EVP_PKEY structure." << std::endl;
          return NULL;
        }

        BIGNUM* exponent = BN_new();
        RSA* rsa = RSA_new();
        if (!pkey || !exponent || !rsa ||
            !BN_set_word(exponent, 0x10001) ||  // 65537 RSA exponent
            !RSA_generate_key_ex(rsa, 1024, exponent, NULL) ||
            !EVP_PKEY_assign_RSA(pkey, rsa)) {
          std::cerr << "Unable to generate 2048-bit RSA key." << std::endl;
          EVP_PKEY_free(pkey);
          return NULL;
        }

        const EVP_MD        * digest;
        unsigned char         md[EVP_MAX_MD_SIZE];
        unsigned int len;
        /* Allocate memory for the X509 structure. */
        X509 * x509 = X509_new();
        if(!x509)
        {
          std::cerr << "Unable to create X509 structure." << std::endl;
          return NULL;
        }

        /* Set the serial number. */
        ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);

        /* This certificate is valid from now until exactly one year from now. */
        X509_gmtime_adj(X509_get_notBefore(x509), 0);
        X509_gmtime_adj(X509_get_notAfter(x509), 31536000L);

        /* Set the public key for our certificate. */
        X509_set_pubkey(x509, pkey);

        /* We want to copy the subject name to the issuer name. */
        X509_NAME * name = X509_get_subject_name(x509);
        /* Set the country code and common name. */
        X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, (unsigned char *)"CA",        -1, -1, 0);
        X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, (unsigned char *)"MyCompany", -1, -1, 0);
        X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"localhost", -1, -1, 0);

        /* Now set the issuer name. */
        X509_set_issuer_name(x509, name);

        /* Actually sign the certificate with our key. */
        if(!X509_sign(x509, pkey, EVP_sha256()))
        {
          std::cerr << "Error signing certificate." << std::endl;
          X509_free(x509);
          return NULL;
        }

        const EVP_MD  *fprint_type = EVP_sha256();
        X509_digest(x509, fprint_type, md, &len);

        return  MakeCertificateFile(pkey,x509);
      }
    }

    bool xGateHmpGstManager::MakeCertificateFile(EVP_PKEY *pkey, X509 *x509)
    {
      struct stat sb;
      char   tmpFile[20];
      char   certFile[100];
      char t[] = TMP_DIR_TEMPLATE;

      String tmpFingerPrt;
      int len;

      g_mkdir_with_parents (ROOT_TMP_DIR, 0777);
      char *folder = mkdtemp (t);
      if( !folder )
      {
        CLog::Detail(THISMODULE, "\n MakeCertificateFile : Failed to create folder\n");
        return false;
      }
      sprintf(certFile,"%s/certkey.pem",folder);
      String tmpStr(certFile);
      sslCertFileName = tmpStr;
      CLog::Detail(THISMODULE,"\n MakeCertificateFile  %s \n",sslCertFileName.c_str());

      /* Open the PEM file for writing the key to disk. */
      FILE * tFileFp = fopen(sslCertFileName.c_str(), "wb");
      if(!tFileFp)
      {
        std::cerr << "Unable to open " << certFile<< " for writing." << std::endl;
        return false;
      }
      /* Write the key to disk. */
      bool ret = PEM_write_PrivateKey(tFileFp, pkey, NULL, NULL, 0, NULL, NULL);

      if(!ret)
      {
        std::cerr << "Unable to write private key to disk." << std::endl;
        return false;
      }

      /* Write the certificate to disk. */
      ret = PEM_write_X509(tFileFp, x509);

      if(!ret)
      {
        std::cerr << "Unable to write certificate to disk." << std::endl;
        return false;
      }

      const EVP_MD  *fprint_type = EVP_sha256();
      X509_digest(x509, fprint_type, tmpFingerPrt.c_str(), &len);

      std::string fingerprint =
        hex_encode_with_delimiter(tmpFingerPrt.c_str(), len, ':');
      std::transform(fingerprint.begin(), fingerprint.end(),
          fingerprint.begin(), ::toupper);

      strcpy(fingerprt,fingerprint.c_str());
      //sprintf(fingerprt, "sha-256 %s",fingerprint.c_str());
      CLog::Detail(THISMODULE, "Fingerprint %s Certificate file %s \n",fingerprt,sslCertFileName.c_str());

      fclose(tFileFp);

      return true;
    }

    void xGateHmpGstManager::removePemFile(string &pemFile)
    {
      size_t pos = pemFile.find_last_of("\\/");
      string pemDir = pemFile.substr(0, pos);

      if(remove(pemFile.c_str()) == 0 ){
        if(rmdir(pemDir.c_str()) == 0 ) {
          CLog::Detail(THISMODULE, " Certificate file %s deleted successfully\n",pemFile.c_str());
        } 
      }
      CLog::Detail(THISMODULE, "Certificate file %s NOT available to delete\n",pemFile.c_str());
    }

#endif
