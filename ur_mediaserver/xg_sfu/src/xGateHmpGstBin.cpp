#include <sys/stat.h>

//ace include
#include <ace/Task.h>
#include <ace/Message_Queue.h>

//self include
#include "xGateHmpGstBin.h"

//includes to get recording file info
#include <stdint.h>
#include <chrono>
#include <thread>
#include <iomanip>
#include <math.h>
#define BUFSIZE 1024

//local include
#include "xglog.h"
#include "xGateSFUtil.h"
#include "SFUClient.h"

#define THISMODULE "HmpBin"

#define DEFAULT_RECORD_SCREEN_WIDTH 640
#define DEFAULT_RECORD_SCREEN_HEIGHT 360

extern SFUClient sfu_client;
/* Client class implementation - Start
*/
Client::Client() :
	m_isConference(FALSE), m_clientCount(0), m_pHmpRecordAudioRecvChannel(NULL),
	m_pHmpRecordVideoRecvChannel(NULL), m_pHmpAudioVideoMuxRecorderChannel(NULL)
{
}

Client::Client(ClientDetail &detail) :
	m_detail(detail), m_isConference(FALSE), m_clientCount(0), m_pHmpRecordAudioRecvChannel(NULL),
	m_pHmpRecordVideoRecvChannel(NULL), m_pHmpAudioVideoMuxRecorderChannel(NULL)
{
}

Client::~Client() 
{
	XGLOG_INFO("Entered Client::~Client() function");
	if(m_pHmpRecordAudioRecvChannel){
		delete m_pHmpRecordAudioRecvChannel;
	}

	if(m_pHmpRecordVideoRecvChannel){
		delete m_pHmpRecordVideoRecvChannel;
	}

	if(m_pHmpAudioVideoMuxRecorderChannel) {
		delete m_pHmpAudioVideoMuxRecorderChannel;
	}
}

// Function to read an atom's size and type
bool readAtom(std::ifstream& file, std::string& type, uint32_t& size) {
  file.read(reinterpret_cast<char*>(&size), sizeof(size));
  size = ntohl(size); // Convert to host byte order
  char atom[4];
  file.read(atom, sizeof(atom));
  type.assign(atom, 4);
  return file.good();
}

string formatDuration(double totalSeconds){
  int hours = static_cast<int>(totalSeconds) / 3600;
  int minutes = (static_cast<int>(totalSeconds) % 3600) / 60;
  int seconds = static_cast<int>(totalSeconds) % 60;
  int milliseconds = static_cast<int>((totalSeconds - static_cast<int>(totalSeconds)) * 1000);
  std::ostringstream duration_ss;
  duration_ss << std::setfill('0') << std::setw(2) << hours << ":"
              << std::setfill('0') << std::setw(2) << minutes << ":"
              << std::setfill('0') << std::setw(2) << seconds << "."
              << std::setfill('0') << std::setw(3) << milliseconds;
  return duration_ss.str();
}

void Client::Fetch_recordfile_info(Client* client)
{
  usleep(1000000); //Introduced to load the file in order to calculate size and duration
  XGLOG_INFO("Got Recorded file: '%s'", client->m_detail.m_record_file.c_str());
  const std::string& filePath = client->m_detail.m_record_file;
  std::string duration_str = "0";
  std::string file_size = "0";
  try {
    auto fileSize = fs::file_size(filePath);
    file_size = std::to_string(fileSize);
  } catch (fs::filesystem_error& e) {
    XGLOG_ERROR("Error getting file size: %s",e.what());
    return;
  }
  std::ifstream file(filePath, std::ios::binary);
  if (!file.is_open()) {
    XGLOG_ERROR("Could not open file: %s",filePath.c_str());
    return;
  }

  std::string type;
  uint32_t size;

  // Locate the 'mvhd' atom
  while (readAtom(file, type, size)) {
    if (type == "moov") {
      while (readAtom(file, type, size)) {
        if (type == "mvhd") {
          file.seekg(12, std::ios_base::cur); // Skip to duration
          uint32_t timeScale, duration;
          file.read(reinterpret_cast<char*>(&timeScale), sizeof(timeScale));
          file.read(reinterpret_cast<char*>(&duration), sizeof(duration));
          timeScale = ntohl(timeScale);
          duration = ntohl(duration);

          double durationInSeconds = static_cast<double>(duration) / timeScale;
          duration_str = formatDuration(durationInSeconds);
          break;
        } else {
            file.seekg(size - 8, std::ios_base::cur); // Move to the next atom
        }
      }
    } else {
        file.seekg(size - 8, std::ios_base::cur); // Move to the next atom
    }
  }
  client->m_detail.m_rec_fileSize = file_size;
  client->m_detail.m_file_duration = duration_str;
  XGLOG_INFO("Calculated Recorded file Size: '%s', Duration: '%s' ", file_size.c_str(), duration_str.c_str());
}

int Client::client_thr_func (void *arg)
{
	ACE_Message_Block *pAmb = 0;
	Client *client = (Client *)arg;
	XGLOG_INFO("Entered Client::client_thr_func client->m_run: %d", client->m_run);
	while(client->m_run)
	{
		int result = client->getq(pAmb, NULL);
		if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
		{
			continue;
		}
		if ((result != -1))
		{
			client->process_client_msg(pAmb);
		}
		else
		{
			XGLOG_ERROR("Client::client_thread_function () Error in Message Queue");
			client->m_run = false;
		}
		// delete the message received
		if(pAmb)
		{
			delete pAmb;
			pAmb = NULL;
		}
	}
	delete client;
	XGLOG_INFO("Client::client_thr_func stopped");
	return 0;
}

gboolean Client::remove_pads_and_elements(GstElement* audio_rtpbin, GstElement* video_rtpbin, unsigned int audio_ssrc, unsigned int video_ssrc)
{
	XGLOG_INFO("Entered remove_pads_and_elements in RTPBIN from AudioRecv and VideoRecv channels");
  // Get the rtpssrcdemux element based on the SSRC
  g_signal_emit_by_name (audio_rtpbin, "clear-ssrc", 0, audio_ssrc);
  g_signal_emit_by_name (video_rtpbin, "clear-ssrc", 0, video_ssrc);
  return TRUE;
}

gboolean Client::Iterate_over_CompositorSinkpads(HmpRecordVideoRecvChannel *channel)
{
	XGLOG_INFO("Entered Iterate_over_CompositorSinkpads");
	HmpRecordVideoRecvChannel *Videorecvchannel = channel;
	GstElement *Compositor = Videorecvchannel->m_pHmpCompositorChannel->m_pHmpVideoCompositorBin->m_compositor;
	guint pad_count = channel->m_pHmpCompositorChannel->m_pHmpVideoCompositorBin->m_compositor->numsinkpads;

	string clientId = channel->m_pHmpCompositorChannel->m_pHmpVideoCompositorBin->m_clientId;
	guint xPos = 0;
	guint yPos = 0;
	guint frameNewWidth = 0, frameNewHeight = 0;
	GstIterator *it = gst_element_iterate_sink_pads(Compositor);
	GValue item = G_VALUE_INIT;

	int row, col = ceil(sqrt(pad_count));
	row = round(sqrt(pad_count));
	if(col > 0)
	  frameNewWidth = DEFAULT_RECORD_SCREEN_WIDTH/col;
	if(row > 0)
	  frameNewHeight = DEFAULT_RECORD_SCREEN_HEIGHT/row; //on every new stream, each existing frame hight, width will be callculated
	for(int i=0; i<row; i++,yPos += frameNewHeight){
		if(i == row-1 && pad_count % col != 0){
			xPos = (frameNewWidth/2)*(col-pad_count%col);
		}
		else
			xPos = 0;
		for(int j=0; j<col && pad_count--; j++, xPos+=frameNewWidth){
			switch(gst_iterator_next(it, &item)) {
				case GST_ITERATOR_OK:
					{
						GstPad *pad = GST_PAD(g_value_get_object(&item));
						if(GST_IS_PAD(pad)){
						  g_object_set(pad, "xpos", xPos, NULL);
						  g_object_set(pad, "ypos", yPos, NULL);
						  g_object_set(pad, "width", frameNewWidth, NULL);
						  g_object_set(pad, "height", frameNewHeight, NULL);
						}
						XGLOG_INFO("Reconfiguring '%u' compositor sink pad at x:y pos '%u:%u'", pad_count, xPos, yPos);
						break;
					}
				case GST_ITERATOR_ERROR:
					{
						XGLOG_ERROR("Recofiguring compositor sink pads for client: '%s' failed !!", clientId.c_str());
						break;
					}
				case GST_ITERATOR_DONE:
					{
						XGLOG_INFO("Reconfigured all compositor sink pad at frame size '%u:%u'", frameNewWidth, frameNewHeight);
						break;
					}
			}
		}
	}
  gst_iterator_free(it);
  return TRUE;
}

bool Client::process_client_msg(ACE_Message_Block *pAmb)
{
  xGateHmpClientThreadMsg *pMsg = static_cast<xGateHmpClientThreadMsg*>(pAmb);
  //XGLOG_INFO("Entered Client:: process_client_msg() msg type: %d",pMsg->m_msgType);
  switch (pMsg->m_msgType) {
	case XGATE_CLIENT_CREATE_CHANNEL:
	{
    XGLOG_INFO("Received msg to create XGATE_CLIENT_CREATE_CHANNEL");
	  Client *client = static_cast<Client *> (pMsg->param2);
	  if(this != client) {
	    XGLOG_ERROR("this pointer and client pointer passed do not match");
		return FALSE;
	  }

		gint audioPort, videoPort = 0;
		GSocket *audioSocket, *videoSocket = NULL;

		audioSocket = allocate_audio_port(audioPort);
		videoSocket = allocate_video_port(videoPort);
		if(!audioPort || !videoPort) {
			XGLOG_ERROR("Audio/Video port is invalid. Creating recording channel for client '%s' call_id:%s failed",m_detail.m_browser_type.c_str(),m_detail.m_callId.c_str());
			handle_send_updated_sdp_msg(client);
			return FALSE;
		}
		m_detail.audioDetail.m_respIp =  xGateUtil::getLocalIpAddr(AF_INET);
		m_detail.audioDetail.m_respPort = audioPort;
		m_detail.videoDetail.m_respIp = xGateUtil::getLocalIpAddr(AF_INET);
		m_detail.videoDetail.m_respPort = videoPort;

		if(m_detail.m_mediaEvent == EN_MEDIA_EVENT_RECORD_START) {
			if(create_recording_channel(audioSocket, audioPort, videoSocket, videoPort)) {
				XGLOG_INFO("Created recording channel with audio:video port '%d:%d' for client: '%s' of type '%s'",
						audioPort, videoPort, m_detail.m_callId.c_str(),m_detail.m_browser_type.c_str());
			}
		}
		handle_send_updated_sdp_msg(client);
	}
	break;
	case XGATE_CLIENT_DELETE_ALL_CHANNEL:
		{
			Client *client = static_cast <Client *> (pMsg->param1);
			if(this != client){
				XGLOG_ERROR("this pointer and client pointer passed do not match");
				break;
			}
			XGLOG_INFO("Received XGATE_CLIENT_DELETE_ALL_CHANNEL for client: %s waiting for thr to join",
					client->m_detail.m_callId.c_str());
			string call_id =  client->m_detail.m_callId.c_str();
			handle_client_stop_channel(call_id.c_str());
			/* Wait for recv and send threads to quit*/
			m_run = FALSE;
		}
		break;
	case XGATE_CLIENT_DELETE_SEND_CHANNEL:
		{
			Client *client = static_cast <Client *> (pMsg->param2);
			char *callId_str = static_cast <char *> (pMsg->param3);
			if(this != client){
				XGLOG_ERROR("this pointer and client pointer passed do not match");
				break;
			}
			XGLOG_INFO("Received XGATE_CLIENT_DELETE_SEND_CHANNEL o_rtp_ep callId %s client_callID: %s",
					callId_str, client->m_detail.m_callId.c_str());
			//handle_client_delete_channel(XGATE_CLIENT_DELETE_SEND_CHANNEL, callId_str);
			/* Free the memory allocated for call string */
			free(callId_str);
		}
		break;
	case XGATE_CLIENT_DELETE_RECV_CHANNEL:
		{
			Client *client = static_cast <Client *> (pMsg->param1);
			if(this != client){
				XGLOG_ERROR("this pointer and client pointer passed do not match");
				break;
			}
			XGLOG_INFO("Received XGATE_CLIENT_DELETE_RECV_CHANNEL for callID: %s",
					client->m_detail.m_callId.c_str());
			//handle_client_delete_channel (XGATE_CLIENT_DELETE_RECV_CHANNEL, NULL);
		}
		break;
	case XGATE_CLIENT_REC_EVENT_REQUEST:
		{
			XGLOG_INFO ("Client::XGATE_CLIENT_REC_EVENT_REQUEST received");
			Client *client = static_cast <Client *> (pMsg->param1);
			client->handle_rec_event_request();
			break;
		}
	case XGATE_CLIENT_SSRC_REMOVE_REQUEST:
		{
			XGLOG_INFO( "handle_channel_req processing EN_MEDIA_EVENT_RECORD_SSRC_REMOVE for audiossrc:%u\
                   and videossrc:%u",  m_detail.m_retrieved_audiossrc,m_detail.m_retrieved_videossrc);
			handle_client_ssrc_remove_request();
			break;
		}
	default:
		{
			XGLOG_ERROR ("Msg type: %d  not handled",pMsg->m_msgType);
			break;
		}
	}
    return TRUE;
}

GstPad* Client::get_rtpbin_ssrc_pad(GstElement *rtpBin, string ssrc){
  GList *pads, *iter;
  if(rtpBin == NULL)
    return;

  pads = rtpBin->srcpads;

  // Iterate over the rtpbin srcpads and get the each pad and compare the pads if retrieved_ssrc is matching or not
  for (iter = pads; iter != NULL; iter = g_list_next(iter)) {
    GstPad *pad = GST_PAD(iter->data);
    if(pad != NULL){
      const char* padName = gst_pad_get_name(pad);
      if(strstr(padName, ssrc.c_str())){
        g_free(padName);
        return pad;
      }
      g_free(padName);
    }
  }
  return NULL;
}

gboolean Client::remove_elements_from_recv_channel(GstPad *srcPad, GstBin *bin) {
  if(srcPad == NULL || bin == NULL)
    return FALSE;

  GstPad *peerSinkPad = NULL;
  peerSinkPad = gst_pad_get_peer(srcPad);

  do{
    GstElement *element = NULL;
    const char* padName = NULL;
    const char* element_name = NULL;

    if(peerSinkPad != NULL){
      gst_pad_unlink(srcPad, peerSinkPad);
      padName = gst_pad_get_name(peerSinkPad);
      element = gst_pad_get_parent_element(peerSinkPad);
    }

    if(element != NULL && padName != NULL){
      if(strstr(padName, "sink_")){ //Returning at audiomixer or compositor sink_%u pad for audio and video recv pipelines
        gst_element_release_request_pad(element, peerSinkPad);
        gst_object_unref(peerSinkPad);
        gst_object_unref(element);
        g_free(padName);
        return TRUE;
      }

      element_name = gst_element_get_name(element);
      if(strstr(element_name,"vp9dec_")){ //Remove the keyframe request probe attached to vp9dec sink pad
        gulong probe_id = (gulong)(guintptr)g_object_get_data(G_OBJECT(peerSinkPad), "pad_probe_id");
        if(probe_id != 0){
          gst_pad_remove_probe(peerSinkPad, probe_id);
          g_object_set_data(G_OBJECT(peerSinkPad), "pad_probe_id",(gpointer)(guintptr)0); //reset probe id
          XGLOG_INFO("Probe removed from sink pad of: %s",GST_ELEMENT_NAME(element));
        }
      }

      if(strstr(GST_PAD_NAME(srcPad), "recv_rtp_src") == NULL)//unref all the pads except rtpbin "recv_rtp_src_%u"
        gst_object_unref(srcPad);
      gst_object_unref(peerSinkPad);
      srcPad = NULL;
      peerSinkPad = NULL;

      if(strstr(element_name, "proxysink")){ //Find prosysink and removing the attached proxysrc from compositor channel
        GstElement *proxysrc = this->proxy_pairs[element];
        if(proxysrc != NULL)
          srcPad = gst_element_get_static_pad(proxysrc,"src");
        if(srcPad != NULL)
          peerSinkPad = gst_pad_get_peer(srcPad);
        this->proxy_pairs.erase(element);
        GstBin *compositor_bin = GST_BIN(gst_element_get_parent(proxysrc));
        gst_bin_remove(compositor_bin, proxysrc);
        gst_element_set_state(proxysrc, GST_STATE_NULL);
        gst_object_unref(G_OBJECT(compositor_bin));
        proxysrc = NULL;
      }
      else{
        srcPad = gst_element_get_static_pad(element,"src");
        if(srcPad != NULL){
          peerSinkPad = gst_pad_get_peer(srcPad);
        }
      }

      gst_bin_remove(bin, element);
      gst_element_set_state(element, GST_STATE_NULL);
      gst_object_unref(element);
      element = NULL;
      g_free(padName);
      g_free(element_name);
    }else{
      XGLOG_ERROR("Failed to retrive element from sinkPad: %s\n",GST_OBJECT_NAME(peerSinkPad));
      return FALSE;
    }
  }while(peerSinkPad != NULL);
  return FALSE;
}

gboolean Client::handle_client_ssrc_remove_request(){

  HmpRecordVideoRecvChannel *pRecordVideochannel = m_pHmpRecordVideoRecvChannel;
  HmpRecordAudioRecvChannel *pRecordAudiochannel = m_pHmpRecordAudioRecvChannel;

  GstElement *audiortpbin = NULL;
  if(pRecordAudiochannel && pRecordAudiochannel->m_pHmpRecordAudioRecvBin)
    audiortpbin = pRecordAudiochannel->m_pHmpRecordAudioRecvBin->m_rtpBin;

  GstElement *videortpbin = NULL;
  if(pRecordVideochannel && pRecordVideochannel->m_pHmpRecordVideoRecvBin)
    videortpbin = pRecordVideochannel->m_pHmpRecordVideoRecvBin->m_rtpBin;

  // For video ssrc_pad removal changes in videorecv channel
  if(m_detail.m_retrieved_videossrc != 0){
    string str_videossrc = to_string(m_detail.m_retrieved_videossrc);
    if(pRecordVideochannel){
      //find the ssrc elements and detach the elements upto compositor in videocompositorchannel sinkpad
      //Get the video recvchannel and video compositor channel to get sink pad of compositor for the ssrc received
      GstBin *videoRecvBin = pRecordVideochannel->m_pHmpRecordVideoRecvBin->m_bin;
      GstPad *ssrc_pad = get_rtpbin_ssrc_pad(videortpbin, str_videossrc);
      if(remove_elements_from_recv_channel(ssrc_pad, videoRecvBin)==TRUE){
      XGLOG_INFO("Successfully removed elements from video channel with ssrc: %s",str_videossrc.c_str());
      Iterate_over_CompositorSinkpads(pRecordVideochannel);
    }else
      XGLOG_ERROR("Failed remove elements from video channel with ssrc: %s",str_videossrc.c_str());
    }
  }
  // For audio ssrc_pad removal changes in audiorecv channel
  if(m_detail.m_retrieved_audiossrc != 0){
    string str_audiossrc = to_string(m_detail.m_retrieved_audiossrc);
    if(pRecordAudiochannel)
    {
      //find the ssrc elements and detach the elements upto audiomixer sinkpad
      GstBin *audioRecvBin = pRecordAudiochannel->m_pHmpRecordAudioRecvBin->m_bin;
      GstPad *ssrc_pad = get_rtpbin_ssrc_pad(audiortpbin, str_audiossrc);
      if(remove_elements_from_recv_channel(ssrc_pad, audioRecvBin)==TRUE)
        XGLOG_INFO("Successfully removed elements from audio channel with ssrc: %s",str_audiossrc.c_str());
      else
        XGLOG_ERROR("Failed remove elements from audio channel with ssrc: %s",str_audiossrc.c_str());
    }
  }

  // Remove pads and elements for the specified SSRC
  if (!remove_pads_and_elements(audiortpbin, videortpbin, m_detail.m_retrieved_audiossrc, m_detail.m_retrieved_videossrc)) {
    XGLOG_ERROR("Failed to remove pads and elements for audio_SSRC %u and video_SSRC %u",
                         m_detail.m_retrieved_audiossrc, m_detail.m_retrieved_videossrc);
    return FALSE;
  }else{
    XGLOG_INFO("Success removed pads and elements for audio_SSRC %u and video_SSRC %u",
                         m_detail.m_retrieved_audiossrc, m_detail.m_retrieved_videossrc);
  }

  GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pHmpRecordVideoRecvChannel->m_pHmpCompositorChannel->m_pipeline),GST_DEBUG_GRAPH_SHOW_ALL,
                                        g_strdup_printf("VideoCompositorChannel_PadRemoval_%u",m_detail.m_retrieved_videossrc));

  GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pHmpRecordVideoRecvChannel->m_pipeline), GST_DEBUG_GRAPH_SHOW_ALL,
                  g_strdup_printf("VideoRecvChannel_ElementRemoval_%u",m_detail.m_retrieved_videossrc));

  GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pHmpRecordAudioRecvChannel->m_pipeline), GST_DEBUG_GRAPH_SHOW_ALL,
                  g_strdup_printf("AudioRecvChannel_ElementRemoval_%u",m_detail.m_retrieved_audiossrc));
  return TRUE;
}


GSocket *Client::allocate_audio_port(gint &audioPort)
{
	GSocket *audioSocket = NULL;
	while(!audioSocket) {
		audioPort = HOSTGST_GET_AUDIO_PORT();
		audioSocket = xGateHmpGstUtil::create_udpsocket(audioPort);
	}
	return audioSocket;
}

GSocket *Client::allocate_video_port(gint &videoPort)
{
	GSocket *videoSocket = NULL;
	while(!videoSocket) {
		videoPort = HOSTGST_GET_AUDIO_PORT();
		videoSocket = xGateHmpGstUtil::create_udpsocket(videoPort);
	}
	return videoSocket;
}

void Client::handle_send_updated_sdp_msg (Client *client)
{
	xGateMgMsgType eMsgType = EN_XGATE_MG_TYPE_UNKNOWN;
	SFUController *pSFUController = xGateSFUtil::getSFUController();

	MgMediaDetail sMediaDetail;
	if (pSFUController){
		sMediaDetail.mgresource_id = client->m_detail.m_mgresourceId;
		sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
		sMediaDetail.joiner_type = client->m_detail.m_joinerType;
		sMediaDetail.call_id = client->m_detail.m_callId;
		sMediaDetail.audioDetail.m_mediaType = EN_MEDIA_SDPINFO_AUDIO;
		sMediaDetail.videoDetail.m_mediaType = EN_MEDIA_SDPINFO_VIDEO;
		sMediaDetail.audioDetail.m_respIp = client->m_detail.audioDetail.m_respIp;
		sMediaDetail.audioDetail.m_respPort = client->m_detail.audioDetail.m_respPort;
		sMediaDetail.videoDetail.m_respIp = client->m_detail.videoDetail.m_respIp;
		sMediaDetail.videoDetail.m_respPort= client->m_detail.videoDetail.m_respPort;
    sMediaDetail.media_event = EN_MEDIA_EVENT_RECORD_START;
		eMsgType = EN_XGATE_MG_VIDEO_REC_MSG_RESPONSE;
		XGLOG_INFO( "Sending EN_XGATE_MG_VIDEO_REC_MSG Response to client: %s: \n",client->m_detail.m_callId.c_str());
		pSFUController->send_msg_sfu_client(&sMediaDetail, client->m_detail, eMsgType);
	}
}

void Client::handle_send_resume_response(Client *client)
{
  xGateMgMsgType eMsgType = EN_XGATE_MG_TYPE_UNKNOWN;
  SFUController *pSFUController = xGateSFUtil::getSFUController();

  MgMediaDetail sMediaDetail;
  if (pSFUController){
    sMediaDetail.mgresource_id = client->m_detail.m_mgresourceId;
    sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
    sMediaDetail.joiner_type = client->m_detail.m_joinerType;
    sMediaDetail.call_id = client->m_detail.m_callId;
    sMediaDetail.media_event = EN_MEDIA_EVENT_RECORD_RESUME;
    eMsgType = EN_XGATE_MG_VIDEO_REC_MSG_RESPONSE;
    XGLOG_INFO( "Sending EN_MEDIA_EVENT_RECORD_RESUME Response to client: %s: \n",client->m_detail.m_callId.c_str());
    pSFUController->send_msg_sfu_client(&sMediaDetail, client->m_detail, eMsgType);
  }
}

void Client::send_keyframe_request(string element_name)
{
  xGateMgMsgType eMsgType = EN_XGATE_MG_TYPE_UNKNOWN;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  Client* client = this;
  int pos = element_name.find('_'); // extracting ssrc from element Ex: "vp9dec_123456789"

  guint video_ssrc = static_cast<guint>(stoul(element_name.substr(pos+1)));

  MgMediaDetail sMediaDetail;
  if (pSFUController){
    sMediaDetail.mgresource_id = client->m_detail.m_mgresourceId;
    sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
    sMediaDetail.joiner_type = client->m_detail.m_joinerType;
    sMediaDetail.call_id = client->m_detail.m_callId;
    sMediaDetail.retrieved_videossrc = video_ssrc;
    eMsgType = EN_XGATE_MG_VIDEO_KEY_FRAME_REQUEST;
    XGLOG_INFO( "Sending EN_XGATE_MG_VIDEO_KEY_FRAME_REQUEST message to client: %s: ssrc: %u",
		            client->m_detail.m_callId.c_str(), sMediaDetail.retrieved_videossrc);
    pSFUController->send_msg_sfu_client(&sMediaDetail, client->m_detail, eMsgType);
  }
}

void Client::set_pipeline_to_pause(){

  HmpAudioVideoMuxRecorderBin *muxBin = NULL;
  if(m_pHmpAudioVideoMuxRecorderChannel)
    muxBin = m_pHmpAudioVideoMuxRecorderChannel->m_pHmpAudioVideoMuxRecorderBin;

  pausedTimestamp = gst_clock_get_time(gst_element_get_clock(m_pHmpRecordVideoRecvChannel->m_pipeline));

  if(muxBin != NULL)
    muxBin->is_pipeline_paused = true;

  if(m_pHmpRecordAudioRecvChannel != NULL){
    m_pHmpRecordAudioRecvChannel->pause();
    XGLOG_INFO("m_pHmpRecordAudioRecvChannel->pause() Paused ...");
  }

  if(m_pHmpRecordVideoRecvChannel != NULL){
    m_pHmpRecordVideoRecvChannel->pause();
    XGLOG_INFO(" m_pHmpRecordVideoRecvChannel->pause() Paused ...");
  }

  if(m_pHmpRecordVideoRecvChannel->m_pHmpCompositorChannel != NULL){
    m_pHmpRecordVideoRecvChannel->m_pHmpCompositorChannel->pause();
    XGLOG_INFO("m_pHmpRecordVideoRecvChannel->m_pHmpCompositorChannel->pause() Paused ...");
  }

  if(m_pHmpAudioVideoMuxRecorderChannel != NULL){
    m_pHmpAudioVideoMuxRecorderChannel->pause();
    XGLOG_INFO("m_pHmpAudioVideoMuxRecorderChannel->pause() Paused ...");
  }
  gst_element_send_event(m_pHmpRecordVideoRecvChannel->m_pipeline, gst_event_new_flush_start());
  gst_element_send_event(m_pHmpRecordAudioRecvChannel->m_pipeline, gst_event_new_flush_start());
  gst_element_send_event(m_pHmpRecordVideoRecvChannel->m_pipeline, gst_event_new_flush_stop(FALSE));
  gst_element_send_event(m_pHmpRecordAudioRecvChannel->m_pipeline, gst_event_new_flush_stop(FALSE));
}

void set_pipeline_running_time(GstElement *pipeline){

  GstClock *clock = gst_pipeline_get_clock(GST_PIPELINE(pipeline));
  GstClockTime absolute_time = gst_clock_get_time(clock);
  GstClockTime base_time = gst_element_get_base_time(pipeline);
  GstClockTime running_time = absolute_time - base_time;
  gst_element_set_start_time(pipeline, running_time);

}

void send_pipeline_segment_event(GstElement* pipeline, GstClockTime clockTime){

  GstSegment segment;
  gst_segment_init(&segment, GST_FORMAT_TIME);
  segment.start = clockTime;
  segment.position = clockTime;
  GstEvent *segment_event_mux = gst_event_new_segment(&segment);
  gst_element_send_event(pipeline, segment_event_mux);

}

void Client::set_pipeline_to_playing(){

  HmpAudioVideoMuxRecorderBin *muxBin = NULL;
  if(m_pHmpAudioVideoMuxRecorderChannel != NULL)
    muxBin = m_pHmpAudioVideoMuxRecorderChannel->m_pHmpAudioVideoMuxRecorderBin;

  HmpRecordVideoRecvBin *videoRecvBin = NULL;
  if(m_pHmpRecordVideoRecvChannel != NULL)
    videoRecvBin = m_pHmpRecordVideoRecvChannel->m_pHmpRecordVideoRecvBin;

  set_pipeline_running_time(m_pHmpRecordVideoRecvChannel->m_pipeline);
  set_pipeline_running_time(m_pHmpRecordAudioRecvChannel->m_pipeline);
  set_pipeline_running_time(m_pHmpRecordVideoRecvChannel->m_pHmpCompositorChannel->m_pipeline);
  set_pipeline_running_time(m_pHmpAudioVideoMuxRecorderChannel->m_pipeline);

  GstClockTime resume_time = gst_clock_get_time(gst_element_get_clock(m_pHmpRecordVideoRecvChannel->m_pipeline));
  totalPauseDuration += resume_time - pausedTimestamp;

  send_pipeline_segment_event(m_pHmpRecordAudioRecvChannel->m_pipeline, totalPauseDuration);
  send_pipeline_segment_event(m_pHmpRecordVideoRecvChannel->m_pipeline, totalPauseDuration);
  send_pipeline_segment_event(m_pHmpAudioVideoMuxRecorderChannel->m_pipeline, totalPauseDuration);

  if(muxBin != NULL)
    muxBin->is_pipeline_paused = false;
  this->start_or_resume_timestamp = gst_util_get_timestamp();

  if(m_pHmpRecordAudioRecvChannel != NULL){
    m_pHmpRecordAudioRecvChannel->start();
    XGLOG_INFO("m_pHmpRecordAudioRecvChannel->start() Started..");
  }

  if(m_pHmpRecordVideoRecvChannel != NULL){
    m_pHmpRecordVideoRecvChannel->start();
    XGLOG_INFO("m_pHmpRecordVideoRecvChannel->start() Started..");
  }

  if(m_pHmpRecordVideoRecvChannel->m_pHmpCompositorChannel != NULL){
    m_pHmpRecordVideoRecvChannel->m_pHmpCompositorChannel->start();
    XGLOG_INFO(" m_pHmpRecordVideoRecvChannel->m_pHmpCompositorChannel->start() Started..");
  }

  if(m_pHmpAudioVideoMuxRecorderChannel != NULL){
    m_pHmpAudioVideoMuxRecorderChannel->start();
    XGLOG_INFO("m_pHmpAudioVideoMuxRecorderChannel->start() Started..");
  }

}

gboolean Client::handle_rec_event_request()
{
	Client *client = this;
	if(!client) {
		XGLOG_ERROR("handle_event, invalid client object !!");
		return FALSE;
	}

	XGLOG_INFO("Handling the handle_rec_event_request : %s ",client->m_detail.m_callId.c_str());
	xGateMediaEvent mediaEvent = client->m_detail.m_mediaEvent;
	switch(mediaEvent)
	{
		case EN_MEDIA_EVENT_RECORD_PAUSE:
			{
        XGLOG_INFO("Client::handle_rec_event_request Recieved recording pause event from MBC");
        set_pipeline_to_pause();
				break;
			}
		case EN_MEDIA_EVENT_RECORD_RESUME:
			{
        XGLOG_INFO("Client::handle_rec_event_request Recieved recording resume event from MBC");
        set_pipeline_to_playing();
        handle_send_resume_response(client);
				break;
			}
	}
	return TRUE;
}

gboolean Client::Set_Record_file()
{
	Client *client = this;
	std::string m_filePath1 = client->m_detail.m_record_file;
	//"/home/nfsshare/VideoRecording/Q5P-n6-O5S-bm/G_3328_xdcfghjhgfc_87450/iIZFmFhMihzJcZF9AAFB_1684767637553.mp4"

	if(m_filePath1.empty()){
	XGLOG_ERROR("Client::Set_Record_file given file path is empty!!");
	return FALSE;
	}
	string nfspath = "/home/nfsshare/VideoRecording/";
	int len = sizeof(nfspath);
	int pos = len-2;
	size_t found = m_filePath1.find_last_of("/");
	string path = m_filePath1.substr(0, found);
	///home/nfsshare/VideoRecording/Q5P-n6-O5S-bm/G_3328_xdcfghjhgfc_87450
	cout<<"path-------------------->"<<path<<endl;

	size_t found1 = path.find_last_of("/");
	// size_t found1 = final_path.find_last_of("/");
	string path1 = path.substr(0, found1);
	cout<<"path1-------------------->"<<path1<<endl;

	string Meeting_Dir = path1;
	//create directory starts - here
	struct stat info;
	if (stat(Meeting_Dir.c_str(), &info) != 0) {
		// Directory does not exist, create it
		mkdir(Meeting_Dir.c_str(), 0777);
	}
	size_t pos1 = path.find_last_of("/");
	// Copy substring after pos
	string sub = path.substr(pos1 + 1);
	cout<<"sub : "<<sub<<endl;

	int lenpath1 = sizeof(path1) - found1;

	string final_str = Meeting_Dir + "/" + sub ;
	cout<<"final_str-------------------->"<<final_str<<endl;

	if (stat(final_str.c_str(), &info) != 0) {
		// Directory does not exist, create it
		mkdir(final_str.c_str(), 0777);
	}
	string Rec_file = m_filePath1.substr(m_filePath1.find_last_of("/") + 1);
	string finalpath = final_str + "/" + Rec_file;
	g_object_set(G_OBJECT(client->m_pHmpAudioVideoMuxRecorderChannel->m_pHmpAudioVideoMuxRecorderBin->m_fileSink), 
			"location", finalpath.c_str(), NULL);
	XGLOG_INFO("Recording file will be stored at : %s",finalpath.c_str());
	return TRUE;
}

void Client::handle_client_stop_channel(char *callId_str)
{
  Client *client = this;

  XGLOG_INFO( "Stopping channels created for the client with callId: %s ",client->m_detail.m_callId.c_str());
  if(client->m_pHmpRecordAudioRecvChannel) {
    XGLOG_INFO( "Stopping channels created for the client->m_pHmpRecordAudioRecvChannel->stop()");
    client->m_pHmpRecordAudioRecvChannel->stop();
  }

  if(client->m_pHmpRecordVideoRecvChannel) {
    XGLOG_INFO( "Stopping channels created for the client->m_pHmpRecordVideoRecvChannel->stop()");
    client->m_pHmpRecordVideoRecvChannel->stop();
  }

  if(client->m_pHmpAudioVideoMuxRecorderChannel) {
    client->m_pHmpAudioVideoMuxRecorderChannel->start();
    GstElement* element = (GstElement*)client->m_pHmpAudioVideoMuxRecorderChannel->m_pHmpAudioVideoMuxRecorderBin->m_bin;
    gst_element_send_event(element,gst_event_new_eos());
  }

  if(client->m_pHmpRecordVideoRecvChannel->m_pHmpCompositorChannel) {
    XGLOG_INFO("Stopping channels created for the client->m_pHmpRecordVideoRecvChannel->m_pHmpCompositorChannel->stop()");
    client->m_pHmpRecordVideoRecvChannel->m_pHmpCompositorChannel->stop();
  }

  client->Fetch_recordfile_info(client);
  client->Post_record_info_to_DB(client);

}

void Client::set_thread_state(gboolean run_state)
{
	m_run = run_state;
}

string Client::form_channel_name(string name)
{
	string channelName = "";
	channelName += name.insert (strlen(name.c_str()), m_detail.m_callId, 0, 10);
	channelName += "_";
	channelName += to_string(m_detail.audioDetail.m_codec);
	channelName += "_";
	channelName += to_string(m_detail.videoDetail.m_codec);
	return channelName; 
}

gboolean Client::create_recording_channel(GSocket *audioSocket, gint audioPort, GSocket *videoSocket, gint videoPort)
{
	string clientId = m_detail.m_callId;
	string Browser_Type = m_detail.m_browser_type;
	XGLOG_INFO("creating recording channel for '%s' client: '%s'", Browser_Type.c_str(), clientId.c_str());

	//create Muxer channel
	if(!create_audiovideo_mix_channel()) {
		XGLOG_ERROR("create_audiovideo_mix_channel for client:-%s failed!!!", m_detail.m_callId.c_str());
		return FALSE;
	}

	//create RecordVideoRecvChannel
	if(!m_pHmpRecordVideoRecvChannel) {
		m_pHmpRecordVideoRecvChannel = new HmpRecordVideoRecvChannel(clientId, "RecordVideoRecvChannel_", EN_PIPELINE_RECORD_AUDIO_RECV);
		if(!m_pHmpRecordVideoRecvChannel->construct_bin("RecordVideoRecvBin_", videoPort, videoSocket,Browser_Type)) {
			XGLOG_INFO("creating RecordVideoRecvChannel for client: '%s' failed!", clientId.c_str());
			delete m_pHmpRecordVideoRecvChannel;
			return FALSE;
		} else {
			m_pHmpRecordVideoRecvChannel->register_rtpbin_callback(this);
			m_pHmpRecordVideoRecvChannel->register_handle_event_callback(this);
		}
	}

	//link video compositor with video mux channel
	HmpVideoCompositorChannel *compositorChannel = m_pHmpRecordVideoRecvChannel->m_pHmpCompositorChannel;
	if(!compositorChannel) {
		XGLOG_ERROR("Video compositor channel not created properly for client: '%s'", clientId.c_str());
		return FALSE;	
	}
  compositorChannel->register_video_buffer_probe_callBack(this, compositorChannel->m_pHmpVideoCompositorBin->m_videoEnc);

	//create RecordAudioRecvChannel
	if(!m_pHmpRecordAudioRecvChannel) {
		m_pHmpRecordAudioRecvChannel = new HmpRecordAudioRecvChannel(clientId, "RecordAudioRecvChannel_", EN_PIPELINE_RECORD_AUDIO_RECV,Browser_Type);
		if(!m_pHmpRecordAudioRecvChannel->construct_bin("RecordAudioRecvBin_", audioPort, audioSocket, Browser_Type)) {
			XGLOG_INFO("creating RecordAudioRecvChannel for client: '%s' failed!", clientId.c_str());
			delete m_pHmpRecordAudioRecvChannel;
			return FALSE;
		} else {
			m_pHmpRecordAudioRecvChannel->register_rtpbin_callback(m_pHmpRecordAudioRecvChannel);
			m_pHmpRecordAudioRecvChannel->register_handle_event_callback(this);
			m_pHmpRecordAudioRecvChannel->register_audio_buffer_probe_callBack(this,
			                              m_pHmpRecordAudioRecvChannel->m_pHmpRecordAudioRecvBin->m_opusEnc);
		}
	}

  GstClock *clock = gst_system_clock_obtain();
  g_object_set(clock, "clock-type", GST_CLOCK_TYPE_MONOTONIC, NULL);
  gst_pipeline_use_clock((GstPipeline*)m_pHmpRecordVideoRecvChannel->m_pipeline, clock);
  gst_pipeline_use_clock((GstPipeline*)m_pHmpRecordAudioRecvChannel->m_pipeline, clock);
  gst_pipeline_use_clock((GstPipeline*)m_pHmpAudioVideoMuxRecorderChannel->m_pipeline, clock);
  gst_pipeline_use_clock((GstPipeline*)compositorChannel->m_pipeline, clock);
	
	m_pHmpAudioVideoMuxRecorderChannel->start();
	this->start_or_resume_timestamp = gst_util_get_timestamp();
	m_pHmpAudioVideoMuxRecorderChannel->draw_dot_file();
	compositorChannel->start();
	compositorChannel->draw_dot_file();
	m_pHmpRecordVideoRecvChannel->start();
	m_pHmpRecordVideoRecvChannel->draw_dot_file();
	m_pHmpRecordAudioRecvChannel->start();
	m_pHmpRecordAudioRecvChannel->draw_dot_file();

	XGLOG_INFO("RecordAudioRecvChannel, RecordVideoRecvChannel and AudioVideoMuxRecorderChannel for client: '%s' created successfully!", 
			clientId.c_str());
	return TRUE;
}

gboolean Client::create_audiovideo_mix_channel()
{
	XGLOG_INFO("create_audiovideo_mix_channel, creating mixer channel for call_id:%s success!!",m_detail.m_callId.c_str());
	Client *client = this;
	string Browser_Type = m_detail.m_browser_type;
	string channelName("AudioVideoMuxRecorderChennal"), binName("AudioVideoMuxRecorderBin");
	HmpChannelType channelType = EN_PIPELINE_RECORD_AUDIO_VIDEO_MUX;
	bool ret = FALSE;

	if(!m_pHmpAudioVideoMuxRecorderChannel){
		m_pHmpAudioVideoMuxRecorderChannel = new HmpAudioVideoMuxRecorderChannel(m_detail.m_callId, channelName, channelType);
		if(!m_pHmpAudioVideoMuxRecorderChannel->construct_bin(binName, Browser_Type)){
			XGLOG_ERROR("create_audiovideo_mix_channel for client:%s failed!!",m_detail.m_callId.c_str());
			return FALSE;
		}
		client->m_pHmpAudioVideoMuxRecorderChannel->register_handle_event_callback(this);
		client->Set_Record_file();
	//	m_pHmpAudioVideoMuxRecorderChannel->start();
	//	m_pHmpAudioVideoMuxRecorderChannel->draw_dot_file();
	}
	else{
		XGLOG_INFO("create_audiovideo_mix_channel for client:-%s created!!!",m_detail.m_callId.c_str());
	}
	return TRUE;
}

gboolean Client::set_media_event_details(xGateMediaEvent mediaEvent, xGateMgMsg* pMgMsg)
{
	MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
	mediaDetail.media_event = mediaEvent;
	switch(mediaEvent) {
		case EN_MEDIA_EVENT_EOS:
			{
				pMgMsg->set_mg_msg_type(EN_XGATE_MG_PLAY_BACK);
				break;
			}
		case EN_MEDIA_EVENT_RECVDIGIT:
			{
				pMgMsg->set_mg_msg_type(EN_XGATE_MG_DTMF_DIGITS);
				mediaDetail.dtmf_digits = m_detail.m_dtmfBuffer;
				m_detail.m_dtmfBuffer.clear();
				break;
			}
		case EN_MEDIA_EVENT_RECORD_INFO:
			{
				pMgMsg->set_mg_msg_type(EN_XGATE_MG_CALL_RECORD);
				mediaDetail.file_size = m_detail.m_fileSize;
				mediaDetail.record_file = m_detail.m_filePath;
				mediaDetail.browser_type = m_detail.m_browser_type;
				break;
			}
		case EN_MEDIA_EVENT_NULL:
		default:
			{
				break;
			}
	}
#if 1 //TODO: we can optimize below code blocks by making ClientDetails and MgMediaDetail make common struct
	mediaDetail.mgresource_id = m_detail.m_mgresourceId;
	mediaDetail.call_type = m_detail.m_callType;
	mediaDetail.call_id = m_detail.m_callId;
	mediaDetail.leg_id = m_detail.m_legId;
	mediaDetail.audioDetail.m_clientIp = m_detail.audioDetail.m_clientIp;
	mediaDetail.audioDetail.m_clientPort = m_detail.audioDetail.m_clientPort;
	mediaDetail.videoDetail.m_clientIp = m_detail.videoDetail.m_clientIp;
	mediaDetail.videoDetail.m_clientPort = m_detail.videoDetail.m_clientPort;
	mediaDetail.call_dir = m_detail.m_callDir;
	mediaDetail.sig_type = m_detail.m_sigType;
#else
	mediaDetail = m_detail;
#endif

	if(m_detail.m_callDir == EN_CALL_DIR_IN) { 
		mediaDetail.audioDetail.m_respIp = m_detail.audioDetail.m_serverIp;
		mediaDetail.audioDetail.m_respPort = m_detail.audioDetail.m_serverPort;
		mediaDetail.videoDetail.m_respIp = m_detail.videoDetail.m_serverIp;
		mediaDetail.videoDetail.m_respPort = m_detail.videoDetail.m_serverPort;
	} else if(m_detail.m_callDir == EN_CALL_DIR_OUT) {
		mediaDetail.audioDetail.m_dialIp = m_detail.audioDetail.m_serverIp;
		mediaDetail.audioDetail.m_dialPort = m_detail.audioDetail.m_serverPort;
		mediaDetail.videoDetail.m_dialIp = m_detail.videoDetail.m_serverIp;
		mediaDetail.videoDetail.m_dialPort = m_detail.videoDetail.m_serverPort;
	}

	return TRUE;
}

gboolean Client::post_media_event_result(xGateMediaEvent mediaEvent)
{
	XGLOG_INFO( "Client::post_pbx_event for client: %s:%d and leg:%d start", \
			m_detail.audioDetail.m_clientIp.c_str(), m_detail.audioDetail.m_clientPort, m_detail.m_legId);
#if 0 //disabled this as part of bug-id: UA-21 fix
	m_detail.m_mediaEvent = mediaEvent; 
#endif

	xGateMgMsg* pMgMsg = new xGateMgMsg();
	xGateNetConnection pbxConInfo;
	pbxConInfo.recvIp_ = m_detail.m_pbxIp;
	pbxConInfo.recvPort_ = m_detail.m_pbxPort;
	pMgMsg->set_net_con_info(pbxConInfo);

	pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
	if(set_media_event_details(mediaEvent, pMgMsg)) {
		SFUClient *sfu_client  = xGateSFUtil::getSFUClient();
		sfu_client->send_msg(pMgMsg);
	}
	return TRUE;
}
/* Client class implementation - End
*/

/*xGateHmpConference class implementation - Start
*/
xGateHmpConference::xGateHmpConference(string mconferenceId, xGateCallType confType) :
	m_conferenceId(mconferenceId), m_confType(confType) 
{
	m_isHost = FALSE;
	m_hostJoined = FALSE;
}

xGateHmpConference::~xGateHmpConference()
{
	remove_participant_entries();
	if(!m_confClientMap.empty()){
		m_confClientMap.clear();      
	}	      
}

xGateRetVal xGateHmpConference::create_channel(ClientDetail &clientDetail)
{
	XGLOG_INFO( "xGateHmpConference::create_channel for m_mgresourceId: %s call-Id %s ", \
			clientDetail.m_mgresourceId.c_str(), clientDetail.m_callId.c_str());

	Client *client = NULL;
	switch(m_confType) {
		case EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO:
			{
				const char *thr_name = "client";
				SFUController *pSFUController = xGateSFUtil::getSFUController();
				xGateHmpConference *pHmpConference = NULL;
				if (!clientDetail.m_callId.c_str() || !clientDetail.m_mgresourceId.c_str()) {
					XGLOG_ERROR("Client details are invalid calliD: %s or resource Id: %s is null",
							clientDetail.m_callId.c_str(), clientDetail.m_mgresourceId.c_str());
					return EN_XGATE_STATUS_ERROR; 
				}

				pHmpConference = pSFUController->find_conference_entry(clientDetail.m_mgresourceId);
				if(!pHmpConference) {
					XGLOG_ERROR("Failed to find entry in conference");
					return EN_XGATE_STATUS_ERROR;
				}

				client = new Client(clientDetail);
				if(!client) {
					XGLOG_ERROR("Failed to allocate memory for new client object");
					return EN_XGATE_STATUS_ERROR; 
				}

				client->set_thread_state (TRUE);
				if(client->m_detail.m_mediaEvent == EN_MEDIA_EVENT_RECORD_START)
				{
					struct timeval time_now;
					long int milli_sec, seconds, useconds;
					gettimeofday(&time_now, NULL);
					seconds = time_now.tv_sec; //seconds
					useconds = time_now.tv_usec; //milliseconds
					milli_sec = ((seconds * 1000) + useconds/1000.0);
					client->m_detail.m_recStartTime = milli_sec;
					XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$ Setting rec start time : %d", client->m_detail.m_recStartTime);
				}

				XGLOG_INFO("Created new client for conf client %p callId: %s mg_resource_id: %s",
						client,client->m_detail.m_callId.c_str(), clientDetail.m_mgresourceId.c_str());
				if (ACE_Thread::spawn(client->client_thr_func,
							client,
							THR_NEW_LWP | THR_JOINABLE | THR_SCHED_RR, //THR_INHERIT_SCHED,
							0,
							0,
							10, /* priority */
							0,
							ACE_DEFAULT_THREAD_STACKSIZE,
							0,
							&thr_name) == -1)
				{
					XGLOG_ERROR("xGateHmpConference:: failed to start client thread !");
					delete client;
					return EN_XGATE_STATUS_ERROR;
				}
				else
				{
					XGLOG_INFO("xGateHmpConference:: Spawning thread success");
				}

				/* post message to create recv channel in client's thread */
				ACE_Message_Block* pAmb = NULL;
				ACE_Time_Value tval ((time(NULL) ));
				tval += ACE_Time_Value(0,1);

				xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg;
				pMsg->m_msgType = XGATE_CLIENT_CREATE_CHANNEL;
				pMsg->param2 = static_cast <void*> (client);
				pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
				if(!client->putq(pAmb, &tval)){
					XGLOG_ERROR("Failed to post message to Client thread to create recv channel");
					delete client;
					return EN_XGATE_STATUS_ERROR;
				}
				pHmpConference->insert_participant_entry(clientDetail.m_callId, client);
				break;
			}
		default:
			{
				XGLOG_ERROR("Invalid callType recvd in create conference client");
				return EN_XGATE_STATUS_ERROR;	 
			}
	}
	return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpConference::add_client(string callId,Client* client)
{	 
	map<string,Client*>::iterator it;	    
	if((it = m_confClientMap.find(callId)) == m_confClientMap.end()) {
		m_confClientMap.insert({callId,client});
		return EN_XGATE_STATUS_SUCCESS;
	}
	XGLOG_ERROR("Found duplicate entry for this callId:%s in pHmpConference",callId.c_str());
	return EN_XGATE_STATUS_ERROR;
}

Client* xGateHmpConference::get_client(string callID)
{
	Client *psClient = NULL;
	std::map<string,Client*>::iterator itr_conf;
	itr_conf = m_confClientMap.find(callID);
	if(itr_conf != m_confClientMap.end()) {
		psClient = itr_conf->second;
	}
	return psClient;
}

xGateRetVal xGateHmpConference::remove_conf_participant(Client *client)
{
	// map<string,HmpRecordingChannel*>::iterator itr_conf;
	// HmpRecordingChannel *pHmpRecordChannel = NULL;
	SFUController *pSFUController = xGateSFUtil::getSFUController();
	ACE_Message_Block *pAmb = 0;
	string callId = client->m_detail.m_callId; 
	xGateHmpClientThreadMsg *pMsg = NULL; 
	// list<map<string,Client*>::iterator> list_conf;

	if(!client) {
		XGLOG_ERROR("No valid client recvd in remove_conf_participant");
		EN_XGATE_STATUS_ERROR;
	}

	/* Fourthly delete the entry for this client from the conference */
	XGLOG_INFO( "Removing the client entry for callId: %s", callId.c_str());
	remove_participant_entry(callId);
	XGLOG_INFO("Sending XGATE_CLIENT_DELETE_ALL_CHANNEL msg to client: %s",
			client->m_detail.m_callId.c_str());
	ACE_Time_Value tval ((time(NULL) ));
	tval += ACE_Time_Value(0,1);
	/* Send delete all channel message from client */ 
	pMsg = new xGateHmpClientThreadMsg ();
	pMsg->m_msgType = XGATE_CLIENT_DELETE_ALL_CHANNEL;
	pMsg->param1 = static_cast <void*> (client);
	pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
	if(!client->putq(pAmb, &tval)) {
		XGLOG_ERROR("Failed to push XGATE_CLIENT_DELETE_ALL_CHANNEL msg to Client thread");
	}

	return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpConference::remove_send_channel(Client *client,
		string callId)
{
	xGateHmpClientThreadMsg *pMsg = NULL;
	ACE_Message_Block *pAmb = 0;
	ACE_Time_Value tval ((time(NULL) ));
	tval += ACE_Time_Value(0,1);
	XGLOG_INFO("Sending XGATE_CLIENT_DELETE_SEND_CHANNEL msg to client: %s",
			client->m_detail.m_callId.c_str());
	pMsg = new xGateHmpClientThreadMsg ();
	char *callId_str = malloc (strlen(callId.c_str()) + 1);
	strcpy (callId_str, callId.c_str());
	pMsg->m_msgType = XGATE_CLIENT_DELETE_SEND_CHANNEL;
	pMsg->param2 = static_cast <void*> (client);
	pMsg->param3 = static_cast <void*> (callId_str);
	pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
	if(!client->putq(pAmb, &tval)){
		XGLOG_ERROR("Failed to push XGATE_CLIENT_DELETE_SEND_CHANNEL msg to Client thread");
		return EN_XGATE_STATUS_ERROR;
	}
	return EN_XGATE_STATUS_SUCCESS;
}

// Start Conference participant MAP related interfaces
Client* xGateHmpConference::get_participant_entry(string callId)
{
	Client* client = NULL;
	CLIENT_MAP::iterator itr = m_clientMap.begin();

	for(itr = m_clientMap.begin(); itr != m_clientMap.end(); itr++) {
		client = itr->second;
		if(client){
			XGLOG_INFO( "Client %p client->m_detail.m_callId.c_str(): %s passed callId: %s",
					client,client->m_detail.m_callId.c_str(), callId.c_str());
		}
		
		if (client->m_detail.m_callId == callId)
			break;
	}
	if(itr != m_clientMap.end()) {
		XGLOG_INFO( "get_participant_entry success for call Id: %s", callId.c_str());
	} else {
		XGLOG_ERROR( "get_participant_entry failed for call Id: %s", callId.c_str());
		return NULL;
	}
	return client;
}

xGateRetVal xGateHmpConference::insert_participant_entry(string callId, Client* client)
{
	xGateRetVal retVal = EN_XGATE_STATUS_ERROR;

	m_clientMap[callId]= client;

	retVal = EN_XGATE_STATUS_SUCCESS;
	return retVal;
}

xGateRetVal xGateHmpConference::remove_participant_entry(string callId)
{
	XGLOG_INFO( "remove_participant_entry for callId: %s", callId.c_str());

	m_clientMap.erase(callId);
	return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpConference::remove_participant_entries()
{
	m_clientMap.clear();

	return EN_XGATE_STATUS_SUCCESS;
}

