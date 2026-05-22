
//local include
#include "xglog.h"
#include "xGateHmpGstChannel.h"
#include "xGateHmpGstBin.h"
#include <cmath>

#define KEYFRAME_INTERVAL_NS (2 * GST_SECOND)  // 2 seconds in nanoseconds

#define THISMODULE "HmpGstChannel"
#define VP8_VIDEO_CODEC  0
#define VP9_VIDEO_CODEC  1

#define CAPS_FILTER_PCMU "audio/x-raw, format=(string)S16LE, rate=(int)8000, channels=(int)1, layout=(string)interleaved"
#define CAPS_FILTER_PCMA "audio/x-raw, format=(string)S16LE, rate=(int)8000, channels=(int)1, layout=(string)interleaved"
#define CAPS_FILTER_G722 "audio/x-raw, format=(string)S16LE, rate=(int)8000, channels=(int)1, layout=(string)interleaved"
#define CAPS_FILTER_G729 "audio/x-raw, format=(string)S16LE, rate=(int)8000, channels=(int)1, layout=(string)interleaved"
#define CAPS_FILTER_OPUS "audio/x-raw, format=(string)S16LE, rate=(int)16000, channels=(int)1, layout=(string)interleaved,sprop-maxcapturerate=(string)8000"
#define CAPS_FILTER_AAC "audio/x-raw, rate=(int)16000, channels=(int)1, format=(string)F32LE, layout=(string)interleaved"
#define CAPS_FILTER_VID_RAW "video/x-raw, format=(string)I420, width=640, height=360, framerate=15/1"
#define CAPS_FILTER_VID_RAW_ENCODE "video/x-raw, format=(string)I420, width=640, height=360,framerate=15/1"

/* HmpGstChannel bass class - Start
*/
HmpGstChannel::HmpGstChannel(string clientId, string pipelineName, HmpChannelType pipelineType) :
	m_clientId(clientId), m_pipelineName(pipelineName), m_pipelineType(pipelineType), 
	m_pipeline(NULL), m_bus(NULL), m_busId(0) 
{
	XGLOG_INFO("HmpGstChannel::HmpGstChannel construction start");
	m_pipelineName.insert(strlen(m_pipelineName.c_str()), m_clientId.c_str(), 0, 10);
	m_pipeline = gst_pipeline_new(m_pipelineName.c_str());
	if(!m_pipeline) {
		XGLOG_ERROR( "HmpGstChannel::HmpGstChannel, %s pipeline object creation failed!");
		return;
	}

	m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
}

HmpGstChannel::~HmpGstChannel()
{
	XGLOG_INFO("HmpGstChannel::~HmpGstChannel destructing %s start", m_pipelineName.c_str());

	if(m_bus) {
		gst_bus_set_sync_handler(m_bus, NULL, NULL, NULL);
		gst_object_unref(GST_OBJECT (m_bus));
	}

	if(m_busId) {
		g_source_remove (m_busId);
	}

#if 1 //TODO: need to properly dispose commanded for call record testing
	if(m_pipeline != NULL) {
		GstState current_state;
                GstStateChangeReturn ret = gst_element_get_state(m_pipeline, &current_state, NULL, GST_CLOCK_TIME_NONE);

                if (ret == GST_STATE_CHANGE_SUCCESS && current_state != GST_STATE_NULL) {
                  // Set the pipeline to NULL state
                  GstStateChangeReturn state_change_ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
                  if (state_change_ret == GST_STATE_CHANGE_FAILURE) {
                    g_printerr("Failed to set pipeline to NULL state in destructor\n");
                  }
                }
		//gst_element_set_state(m_pipeline, GST_STATE_NULL);
		gst_object_unref (GST_OBJECT (m_pipeline));
		XGLOG_INFO("HmpGstChannel::~HmpGstChannel  unreffing the m_pipeline!!");
		m_pipeline = NULL;
	}
#endif
}

gboolean HmpGstChannel::register_handle_event_callback(gpointer data)
{
  XGLOG_INFO("HmpGstChannel::register_handle_event_callback registor!!!");
	if(m_pipeline && m_bus && data) {
		m_busId = gst_bus_add_watch(m_bus, &handle_event, data); 
		return ((int)m_busId > 0) ? TRUE : FALSE;
	} 
	return FALSE;
}

static GstPadProbeReturn HmpRecordVideoRecvBin::vp9dec_sink_buffer_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
  GstEvent *event = GST_PAD_PROBE_INFO_EVENT(info);
  Client *client = (Client*)user_data;

  if(!client) return GST_PAD_PROBE_OK;

  GstClockTime current_time = gst_util_get_timestamp();  // Get current GStreamer timestamp

  if ((current_time - client->start_or_resume_timestamp) >= KEYFRAME_INTERVAL_NS){
    if(event != NULL && GST_EVENT_TYPE(event) == GST_EVENT_CUSTOM_UPSTREAM) {
      const GstStructure *structure = gst_event_get_structure(event);
      if(gst_structure_has_name(structure, "GstForceKeyUnit")) {
        GstElement *vp9dec = gst_pad_get_parent_element(pad);
        if(vp9dec != NULL){
          string element_name = gst_element_get_name(vp9dec);
          XGLOG_INFO("Calling send_keyframe_request: %s",element_name.c_str());
          client->send_keyframe_request(element_name);
          gst_object_unref(vp9dec);
        }
      }
    }
  }
  return GST_PAD_PROBE_OK;
}

gboolean HmpGstChannel::handle_event(GstBus* bus, GstMessage* msg, gpointer data)
{
	Client *client = (Client *)data;
	if(!client) {
		XGLOG_ERROR( "handle_event, invalid client object !!");
		return TRUE;
	}

	switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR:
			{
				XGLOG_INFO( "handle_event got GST_MESSAGE_ERROR message");
				GError *err = NULL;
				gchar *dbgInfo = NULL;
				if(HAVE_GST_DEBUG_PRINTS) 
				{
					gst_message_parse_error(msg, &err, &dbgInfo);
					if(err) {
						g_printerr("ERROR from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
						XGLOG_ERROR( "ERROR from element %s: %s", GST_OBJECT_NAME(msg->src), err->message);
						g_error_free(err);
					}
					if(dbgInfo) {
						g_printerr("Debugging info: %s\n", (dbgInfo) ? dbgInfo : "none");
						XGLOG_ERROR( "Debugging info: %s\n", dbgInfo);
						g_free(dbgInfo);
					}
				}
				break;
			}
		case GST_MESSAGE_EOS:
			{
				string &playFile = client->m_detail.audioDetail.m_playFile;
				XGLOG_INFO( "handle_event got GST_MESSAGE_EOS message file!!!");
				xGateMediaEvent mediaEvent = client->m_detail.m_mediaEvent;
				if(client->m_pHmpAudioVideoMuxRecorderChannel) {
					client->m_pHmpAudioVideoMuxRecorderChannel->stop();
				}
				break;
			}
		case GST_MESSAGE_ELEMENT:
			{
				const GstStructure* gstStruct = gst_message_get_structure(msg);
				const gchar *structName = gst_structure_get_name(gstStruct);
				break;
			}
		case GST_MESSAGE_UNKNOWN:
		default:
			{
				//XGLOG_INFO( "handle_event got GST_MESSAGE_UNKNOWN/DEFAULT message");
				break;
			}
	}
	return TRUE; //it's important to return TRUE from this function
}

gboolean HmpGstChannel::draw_dot_file()
{
	gboolean retVal = FALSE;
	if(HAVE_GST_DEBUG_PRINTS && m_pipeline && !m_pipelineName.empty()) {
		GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_pipelineName.c_str());
		retVal = TRUE;
	}
	return retVal;
}

gboolean HmpGstChannel::start()
{
	gboolean retVal = FALSE;
	if(m_pipeline) {
		gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
		retVal = TRUE;
	}
	//draw_dot_file();
	return retVal;
}

gboolean HmpGstChannel::pause()
{
 gboolean retVal = FALSE;
 if(m_pipeline) {
   gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
   retVal = TRUE;
 }
 //draw_dot_file();
 return retVal;
}

gboolean HmpGstChannel::stop()
{
	gboolean retVal = FALSE;
	if(m_pipeline) {
		gst_element_set_state(m_pipeline, GST_STATE_NULL);
		retVal = TRUE;
	}
	//draw_dot_file();
	return retVal;
}
/* HmpGstChannel bass class - End
*/

/* HmpGstBin Base class - Start
*/
//constructor and destuctor
HmpGstBin::HmpGstBin(string clientId, string binName) : 
	m_clientId(clientId), m_binName(binName), m_bin(NULL)
{
	m_binName.insert(strlen(m_binName.c_str()), m_clientId.c_str(), 0, 10);
	m_bin = GST_BIN(gst_bin_new(binName.c_str()));
}

HmpGstBin::~HmpGstBin()
{
	XGLOG_INFO("HmpGstBin::~HmpGstBin start for bin: %s", m_binName.c_str());
}

gboolean HmpGstBin::draw_dot_file()
{
	if(HAVE_GST_DEBUG_PRINTS && m_bin && !m_binName.empty()) {
		GST_DEBUG_BIN_TO_DOT_FILE(m_bin, GST_DEBUG_GRAPH_SHOW_ALL, m_binName.c_str());
		return TRUE;
	}
	return FALSE;
}
/* HmpGstBin Base class - Start
*/

/* HmpRecordAudioRecvBin class - Start
*/
HmpRecordAudioRecvBin::HmpRecordAudioRecvBin(string clientId, string binName, gint recvPort, GSocket *recvSock, string browser_type) 
	: HmpGstBin(clientId, binName), m_udpSrc(NULL), m_rtpBin(NULL), m_audioMix(NULL), m_opusEnc(NULL),
	m_recvPort(recvPort), m_recvSocket(recvSock), m_capsfilter(NULL)
{
	Browser_Type = browser_type;
	XGLOG_INFO( "HmpRecordAudioRecvBin::HmpRecordAudioRecvBin construction start");
}

HmpRecordAudioRecvBin::~HmpRecordAudioRecvBin()
{
	if(m_recvSocket) {
		g_socket_close(m_recvSocket, NULL);
		g_object_unref(G_OBJECT(m_recvSocket));
		m_recvSocket = NULL;
		m_recvPort = 0;
	}
	XGLOG_INFO("HmpRecordAudioRecvBin destructed");
}

void HmpRecordAudioRecvBin::safe_clean()
{
	if(m_udpSrc) {
		gst_object_unref(GST_OBJECT(m_udpSrc));
		m_udpSrc = NULL;
	}
	if(m_rtpBin) {
		gst_object_unref(GST_OBJECT(m_rtpBin));
		m_rtpBin = NULL;
	}
	if(m_audioMix) {
		gst_object_unref(GST_OBJECT(m_audioMix));
		m_audioMix = NULL;
	}
	if(m_opusEnc) {
		gst_object_unref(GST_OBJECT(m_opusEnc));
		m_opusEnc = NULL;
	}
	XGLOG_INFO( "Safely cleaned HmpRecorderAudioBin");
}

gboolean HmpRecordAudioRecvBin::construct()
{
	//create udpsrc element and bind socket
	if(!create_udpsrc()) {
		XGLOG_WARN("Creating udpsrc failed !");
		goto return_on_fail;
	}

	//create rtpbin element
	if(!create_rtpbin()) {
		XGLOG_WARN("Creating rtpbin failed !");
		goto return_on_fail;
	}

	gst_bin_add_many(m_bin, m_udpSrc, m_rtpBin, NULL);
	if(FALSE == gst_element_link_many(m_udpSrc, m_rtpBin, NULL)) {
		XGLOG_WARN("Linking udpsrc and rtpbin elements failed !");
		goto return_on_fail;
	}

	//create and link audio mixing element
	if(!setup_audiomixer()) {
		XGLOG_WARN("Setup audiomixer failed !");
		goto return_on_fail;
	}

	XGLOG_INFO("HmpRecordAudioRecvBin constructed successfully.");
	return TRUE;

return_on_fail:
	XGLOG_ERROR("HmpRecordAudioRecvBin construction failed !");
	return FALSE;
}

gboolean HmpRecordAudioRecvBin::setup_audiomixer()
{
	m_audioMix = gst_element_factory_make("audiomixer", NULL);
	m_capsfilter = gst_element_factory_make("capsfilter", NULL);
	m_opusEnc = gst_element_factory_make("avenc_aac", NULL);
	GstCaps *filter_caps = gst_caps_from_string(CAPS_FILTER_AAC);
	GstElement *q = gst_element_factory_make("queue2", NULL);

	if(!m_audioMix && !m_capsfilter && !q && !m_opusEnc) {
		XGLOG_WARN("Creating audiomixer or m_capsfilter or avenc_aac or appsink elements failed !");
		goto return_on_fail;
	}

	g_object_set (G_OBJECT(m_opusEnc) ,"bitrate", 32000, NULL);

	// Setting the caps property in capsfilter
	g_object_set(G_OBJECT(m_capsfilter), "caps",filter_caps, NULL);
	gst_caps_unref(filter_caps);

	XGLOG_INFO("HmpRecordAudioRecvBin::setup_audiomixer HmpRecordAudioRecvBin::setup_audiomixer[%s]",Browser_Type.c_str());

	// configure important parameters of audiomixer 
	g_object_set (G_OBJECT(m_audioMix),"output-buffer-duration", 20000000,
                                     "start-time-selection", 1,
                                     "ignore-inactive-pads", FALSE, NULL);


	gst_bin_add_many(m_bin, m_audioMix, m_capsfilter, q, m_opusEnc,  NULL);
	if(FALSE == gst_element_link_many(m_audioMix, m_capsfilter, q, m_opusEnc, NULL)) {
		XGLOG_WARN("Linking audio mixer elements failed !");
		goto return_on_fail;
	}

	XGLOG_INFO("HmpRecordAudioRecvBin setup audio mixer successfully.");
	return TRUE;

return_on_fail:
	XGLOG_ERROR("HmpRecordAudioRecvBin setup audio mixer failed !");
	return FALSE;
}

gboolean HmpRecordAudioRecvBin::create_udpsrc()
{
	XGLOG_INFO( "create_udpsrc start");
	if(!m_udpSrc) {
		m_udpSrc = gst_element_factory_make("udpsrc", NULL);
		if(m_udpSrc) {
			if(m_recvSocket) {
				g_object_set(G_OBJECT(m_udpSrc), "socket", m_recvSocket, NULL);
			} else {
				XGLOG_ERROR( "create_udpsrc failed due to invalid udp socket !!"); 
				return FALSE;
			}

			//GstCaps* caps = get_caps();
			GstCaps* caps = gst_caps_from_string("application/x-rtp");
			if(caps) {
				g_object_set(G_OBJECT(m_udpSrc), "caps", caps, NULL);
				gst_caps_unref(caps);
			} else {
				XGLOG_ERROR( "create_udpsrc setting caps for udpsrc failed !!"); 
				return FALSE;
			}
			g_object_set(G_OBJECT(m_udpSrc), "timeout", 0, "reuse", TRUE, "close-socket", FALSE, NULL);
		} else {
			XGLOG_ERROR( "create_udpsrc gst_element_make udpsrc failed !!");
			return FALSE;
		}
	}
	return TRUE;
}

gboolean HmpRecordAudioRecvBin::create_rtpbin()
{
	XGLOG_INFO( "HmpRecordAudioRecvBin::create_rtpbin start");
	guint latency = 40;
	m_rtpBin = gst_element_factory_make("rtpbin", NULL);
	if(!m_rtpBin) {
		XGLOG_ERROR( "HmpRecordAudioRecvBin::create_rtpbin gst_element_make rtpbin failed !!");
		return FALSE;
	}

	g_object_set(G_OBJECT(m_rtpBin),"autoremove", TRUE, NULL);
	g_object_set(G_OBJECT(m_rtpBin), "latency", latency, NULL);
	return TRUE;
}

gboolean HmpRecordAudioRecvBin::register_rtpbin_callback(gpointer data)
{
	if(m_rtpBin) {
		g_signal_connect(G_OBJECT(m_rtpBin), "pad-added", G_CALLBACK(&on_pad_added), data);
		//g_signal_connect(G_OBJECT(m_rtpBin), "pad-removed", G_CALLBACK(&on_pad_removed), data);
		g_signal_connect(G_OBJECT(m_rtpBin), "request-pt-map", G_CALLBACK(&on_request_pt_map), data);
		return TRUE;
	}
	return FALSE;
}

void HmpRecordAudioRecvBin::on_pad_added(GstElement *rtpBin, GstPad *pad, gpointer data)
{
	gchar *padName = NULL;
	GstPadDirection padDir = GST_PAD_UNKNOWN; 
	guint sn, ssrc, pt = 0;

	padName = gst_pad_get_name(pad);
	sscanf(padName, "recv_rtp_src_%u_%u_%u", &sn, &ssrc, &pt); //sample pad name "recv_rtp_src_0_4239506697_8"
	XGLOG_INFO( "HmpRecordAudioRecvBin::on_pad_added triggered for pad: '%s'", padName);

	HmpRecordAudioRecvChannel *channel = (HmpRecordAudioRecvChannel *)data;
	if(!channel) {
		XGLOG_WARN("Invalid HmpRecordAudioRecvChannel pointer received for payload '%u'", pt);
		goto return_on_fail;
	}
	padDir = gst_pad_get_direction(pad);
	switch(padDir) {
		case GST_PAD_SRC:
			{
				//create and attach depay attach elements
				if(!attach_depay_decoder(pad, pt, channel)) {
					XGLOG_WARN("attaching depay and decoder elements for payload '%u'", pt);
					goto return_on_fail;
				}
				break;
			}
		case GST_PAD_SINK: 
			{
				goto return_on_fail;
			}
		case GST_PAD_UNKNOWN:
		default:
			{
				goto return_on_fail;
			}
	}

	channel->draw_dot_file(); 

	g_free(padName);
	XGLOG_INFO( "handling on_pad_added event for payload '%u' success!", pt);
	return;

return_on_fail:
	g_free(padName);
	XGLOG_ERROR( "handling on_pad_added event for payload '%u' failed!", pt);
}

static GstPadProbeReturn cb_have_data (GstPad *pad, GstPadProbeInfo *info, GstElement *filter){
  GstBuffer *buffer;
  buffer = GST_PAD_PROBE_INFO_BUFFER (info);
  buffer = gst_buffer_make_writable (buffer);

  if (buffer == NULL)
  {
    return GST_PAD_PROBE_OK;
  }
  
  //g_print("Buffer size '%d' and pad of capsfilter caps:- '%s'\n",sizeof(buffer), gst_caps_to_string(gst_pad_get_current_caps(pad)));

  // Setting the PTS and DTS to the buffer for the sync data
  GstClockTime clocktime = 0;
  clocktime = gst_element_get_current_running_time(filter);
  GST_BUFFER_DTS (buffer) = clocktime;
  GST_BUFFER_PTS (buffer) = clocktime;

  return GST_PAD_PROBE_OK;
}

gboolean HmpRecordAudioRecvBin::attach_depay_decoder(GstPad *pad, gint pt, HmpRecordAudioRecvChannel *channel)
{
	XGLOG_INFO( "creating depay and decoder elements to handle payload '%u'", pt);
	GstElement *q, *depay, *decoder, *audio_resample, *convert, *filter ;
	q = NULL;depay = NULL; decoder = NULL; audio_resample = NULL; convert = NULL; filter = NULL;
	GstCaps  *caps = NULL;
	GstPadTemplate *tmpl;
	GstPad* sinkpad = NULL;
	string Browser_Type = channel->Browser_Type;
	HmpRecordAudioRecvBin *recvBin = channel->m_pHmpRecordAudioRecvBin;
	q = gst_element_factory_make("queue", NULL);	
	g_object_set(G_OBJECT(q), "max-size-bytes", 2097152, NULL);
	g_object_set(G_OBJECT(q), "max-size-buffers", 5, NULL);

	// if(Browser_Type.empty()){
	// 	return FALSE;
	// }

	audio_resample = gst_element_factory_make("audioresample", NULL);
	if(!Browser_Type.empty() && (strstr(Browser_Type.c_str(),"Mac") || strstr(Browser_Type.c_str(),"IOS"))){
		convert = gst_element_factory_make("audioconvert", NULL);
	}
	g_object_set(G_OBJECT(audio_resample),"quality",8,NULL);
	filter = gst_element_factory_make("capsfilter", NULL);	
	if(!q && !filter) {
		XGLOG_ERROR("Invalid queue or appsink or filter element!");
		goto return_on_fail;
	}

	switch(pt) 
	{
		case EN_CODEC_PCMU:
			{
				depay = gst_element_factory_make("rtppcmudepay", NULL);
				decoder = gst_element_factory_make("mulawdec", NULL);
				caps = gst_caps_from_string(CAPS_FILTER_PCMU);
				break;
			}
		case EN_CODEC_PCMA:
			{
				depay = gst_element_factory_make("rtppcmadepay", NULL);
				decoder = gst_element_factory_make("alawdec", NULL);
				caps = gst_caps_from_string(CAPS_FILTER_PCMA);
				break;
			}
		case EN_CODEC_G722:
			{
				depay = gst_element_factory_make("rtpg722depay", NULL);
				decoder = gst_element_factory_make("avdec_g722", NULL);
				caps = gst_caps_from_string(CAPS_FILTER_G722);
				break;
			}
		case EN_CODEC_G729:
			{
				depay = gst_element_factory_make("rtpg729depay", NULL);
				decoder = gst_element_factory_make("avdec_g729", NULL);
				caps = gst_caps_from_string(CAPS_FILTER_G729);
			}
		case 96 ... 127: //for opus dynamic payload
			{
				depay = gst_element_factory_make("rtpopusdepay", NULL);	
				decoder = gst_element_factory_make("opusdec", NULL);	
				caps = gst_caps_from_string(CAPS_FILTER_OPUS);
				break;
			}
		case EN_CODEC_CN:
			{
				XGLOG_WARN("Not not supporting comfort noise payload '%u' !", pt);
				goto return_on_fail;
			}
		default:
			{
				XGLOG_WARN("ignoring not supporting payload '%u' !", pt);
				goto return_on_fail;
			}
	} 

	if(!depay && !decoder && !filter && !audio_resample) {
		XGLOG_WARN("Invalid depay or decoder or filter element.");
		goto return_on_fail;
	}

	if(!caps) {
		XGLOG_WARN("Invalid gst caps!");
		goto return_on_fail;
	} else {
		g_object_set(G_OBJECT(filter), "caps", caps, NULL);
    gst_caps_unref(caps);
		caps = NULL;
	}

	if(!recvBin->m_audioMix) {
		XGLOG_WARN("Invalid audio mixer element in HmpRecordAudioRecv channel.");
		goto return_on_fail;
	}

	//add decoder elements
	if(!Browser_Type.empty() && (strstr(Browser_Type.c_str(),"Mac") || strstr(Browser_Type.c_str(),"IOS"))){
		XGLOG_INFO("Maccccccccccccccccccccccccccccccccccccccccccccccc");
	gst_bin_add_many(recvBin->m_bin, q, depay, decoder, convert, audio_resample, filter, NULL);
	if(FALSE == gst_element_link_many(q, depay, decoder, convert, audio_resample, filter, NULL)) {
		gst_bin_remove_many(recvBin->m_bin, q, depay, decoder, convert, audio_resample, filter, NULL);
		XGLOG_WARN("Linking depay, decoder and filter elements failed.");
		XGLOG_ERROR("Attaching depayloader and decoder in HmpRecordAudioRecv channel to handle payload '%u' failed!", pt);
		return FALSE;
	}
	}else{
		XGLOG_INFO("Winnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
	gst_bin_add_many(recvBin->m_bin, q, depay, decoder, audio_resample,filter, NULL);
	if(FALSE == gst_element_link_many(q, depay, decoder, audio_resample, filter, NULL)) {
		gst_bin_remove_many(recvBin->m_bin, q, depay, decoder, audio_resample, filter, NULL);
		XGLOG_WARN("Linking depay, decoder and filter elements failed.");
		XGLOG_ERROR("Attaching depayloader and decoder in HmpRecordAudioRecv channel to handle payload '%u' failed!", pt);
		return FALSE;
	}
	}

	//linking decoder filter with audio mixer sink pad
	tmpl = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(recvBin->m_audioMix),"sink_%u");
	if(tmpl) {
		GstPad *srcpad = gst_element_get_static_pad(filter, "src");
		GstPad *sinkpad = gst_element_request_pad(recvBin->m_audioMix, tmpl, NULL, NULL);
		gst_pad_link(srcpad, sinkpad);
		gst_object_unref(GST_OBJECT(srcpad));
		gst_object_unref(GST_OBJECT(tmpl));
	} else {
		XGLOG_ERROR("Attaching depayloader and decoder in HmpRecordAudioRecv channel to handle payload '%u' failed!", pt);
		return FALSE;
	}

	//linking decoder queue with rtpbin dynamic src pad
	sinkpad = gst_element_get_static_pad(q, "sink");
	gst_pad_link(pad, sinkpad);
	gst_object_unref(GST_OBJECT(sinkpad));

	// Registering the GstPadProbeCallback to sync the timestamp of the buffer
        gst_pad_add_probe (gst_element_get_static_pad(filter, "sink"), GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback) cb_have_data,filter, NULL);

	//synch newly added elements with parent
	gst_element_sync_state_with_parent(filter);
	gst_element_sync_state_with_parent(audio_resample);
	gst_element_sync_state_with_parent(decoder);
	if(convert){
	gst_element_sync_state_with_parent(convert);
	}
	gst_element_sync_state_with_parent(depay);
	gst_element_sync_state_with_parent(q);

	XGLOG_INFO("Attaching depayloader and decoder in HmpRecordAudioRecv channel to handle payload '%u' success!", pt);
	return TRUE;

return_on_fail:
	if(q) gst_object_unref(GST_OBJECT(q));
	if(depay) gst_object_unref(GST_OBJECT(depay));
	if(decoder) gst_object_unref(GST_OBJECT(decoder));
  	if(convert)	gst_element_sync_state_with_parent(convert);
	if(filter) gst_object_unref(GST_OBJECT(filter));
	if(audio_resample) gst_object_unref(GST_OBJECT(audio_resample));
	if(caps) gst_caps_unref(caps);
	XGLOG_ERROR("Attaching depayloader and decoder in HmpRecordAudioRecv channel to handle payload '%u' failed!", pt);
	return FALSE;
}

GstCaps * HmpRecordAudioRecvBin::on_request_pt_map(GstElement *rtpBin, guint session, guint pt, gpointer data)
{
	XGLOG_INFO( "HmpRecordAudioRecvBin::on_request_pt_map triggered for pt %d", pt);
	GstCaps * caps = NULL;
	switch(pt) {
		case 0:
			{
				caps = gst_caps_new_simple("application/x-rtp",
						"media",G_TYPE_STRING,"audio",
						"clock-rate",G_TYPE_INT,8000,
						NULL);
				break;
			}
		case 8:
			{
				caps = gst_caps_new_simple("application/x-rtp",
						"media",G_TYPE_STRING,"audio",
						"clock-rate",G_TYPE_INT,8000,
						NULL);
				break;
			}
		case 9:
			{
				caps = gst_caps_new_simple("application/x-rtp",
						"media",G_TYPE_STRING,"audio",
						"clock-rate",G_TYPE_INT,8000,
						NULL);
				break;
			}
		case 13:
			{
				caps = gst_caps_new_simple("application/x-rtp",
						"media",G_TYPE_STRING,"audio",
						"clock-rate",G_TYPE_INT,8000,
						NULL);
				break;
			}
		case 18:
			{
				caps = gst_caps_new_simple("application/x-rtp",
						"media",G_TYPE_STRING,"audio",
						"clock-rate",G_TYPE_INT,8000,
						NULL);
				break;
			}
		case 96 ... 127: //for opus dynamic payload
			{
				caps = gst_caps_new_simple("application/x-rtp",
						"media",G_TYPE_STRING,"audio",
						"clock-rate",G_TYPE_INT,48000,
						"encoding-name",G_TYPE_STRING,"X-GST-OPUS-DRAFT-SPITTKA-00",
						NULL);
				break;
			}
		default:
			{
				caps = gst_caps_new_simple("application/x-rtp",
						"media",G_TYPE_STRING,"audio",
						"clock-rate",G_TYPE_INT,48000,
						"encoding-name",G_TYPE_STRING,"X-GST-OPUS-DRAFT-SPITTKA-00",
						NULL);
				XGLOG_WARN( "on_request_pt_map, no caps setting for unsupported payload type: %u, pt: %u", session, pt);
				break;
			}
	}
	return caps;
}

gboolean HmpRecordAudioRecvBin::register_audio_buffer_probe_callBack (gpointer data, GstElement *element)
{
 gulong probeId = 0;
 GstPad *srcPad = NULL;
 if(element){
   srcPad = gst_element_get_static_pad (element, "src");
   if (srcPad){
     probeId = gst_pad_add_probe (srcPad,(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_PUSH ),
                                handle_AudioPadProbeCallback, data, NULL);
     XGLOG_INFO("register_audio_buffer_probe_callBack generated buffer probe_Id: %lu ", probeId);
     gst_object_unref(srcPad);
     return TRUE;
   } else {
     XGLOG_ERROR ("Failed to get src pad for capsfilter element");
   }
 }
 gst_object_unref(srcPad);
 return FALSE;
}

GstPadProbeReturn HmpRecordAudioRecvBin::handle_AudioPadProbeCallback (GstPad * pad,
                                                         GstPadProbeInfo * ProbeInfo,
                                                         gpointer data)
{
  Client *client = (Client*)data;
  GstBuffer *buffer = NULL;
  buffer = gst_pad_probe_info_get_buffer (ProbeInfo);
  HmpAudioVideoMuxRecorderBin *pHmpAudioVideoMuxRecorderBin = NULL;
  pHmpAudioVideoMuxRecorderBin = client->m_pHmpAudioVideoMuxRecorderChannel->m_pHmpAudioVideoMuxRecorderBin;
  GstElement* appsrc = pHmpAudioVideoMuxRecorderBin->m_audioSrc;
  GstFlowReturn ret = GST_FLOW_ERROR;
  GstClockTime now = 0;

  if(pHmpAudioVideoMuxRecorderBin->is_pipeline_paused){
    return GST_PAD_PROBE_DROP;
  }

  if (!buffer){
    XGLOG_ERROR ("Buffer is empty ");
    goto return_from_func;
  }

  now = gst_element_get_current_running_time(appsrc);
  GST_BUFFER_PTS(buffer) = now;
  GST_BUFFER_DTS(buffer) = now;

  if(pHmpAudioVideoMuxRecorderBin->set_audiocaps && appsrc){
    GstCaps *audio_caps = gst_pad_get_current_caps(pad);
    gst_app_src_set_caps(GST_APP_SRC(appsrc), audio_caps);
    XGLOG_INFO("Successfully set the audio Appsrc caps!!!");
    gst_caps_unref(audio_caps);
    pHmpAudioVideoMuxRecorderBin->set_audiocaps = false;
  }

  ret = gst_app_src_push_buffer(GST_APP_SRC(appsrc),buffer);

  if(GST_FLOW_OK != ret){
    XGLOG_ERROR("Failed to push buffer to the audio Appsc!!!");
  }
return_from_func:
  return GST_PAD_PROBE_HANDLED ;
}

/* HmpRecordAudioRecvBin class - End
*/

/* HmpRecordAudioRecvChannel derived class - Start
*/
HmpRecordAudioRecvChannel::HmpRecordAudioRecvChannel(string clientId, string pipelineName, HmpChannelType pipelineType, string browser_type) :
	HmpGstChannel(clientId, pipelineName, pipelineType) , m_pHmpRecordAudioRecvBin(NULL)
{
	Browser_Type = browser_type;
	XGLOG_INFO( "HmpRecordAudioRecvChannel::HmpRecordAudioRecvChannel construction start");
}

HmpRecordAudioRecvChannel::~HmpRecordAudioRecvChannel()
{
	XGLOG_INFO( "HmpRecordAudioRecvChannel::~HmpRecordAudioRecvChannel destruction start");
	if(m_pHmpRecordAudioRecvBin) {
		delete m_pHmpRecordAudioRecvBin;
	}
}

gboolean HmpRecordAudioRecvChannel::construct_bin(string binName, gint recvPort, GSocket *recvSock, string Browser_Type)
{
	XGLOG_INFO( "HmpRecordAudioRecvChannel::construct_bin %s start", binName.c_str());
	if(!m_pHmpRecordAudioRecvBin) {
		m_pHmpRecordAudioRecvBin = new HmpRecordAudioRecvBin(m_clientId, binName, recvPort, recvSock, Browser_Type); 
		if(!m_pHmpRecordAudioRecvBin->construct()) {
			XGLOG_ERROR( "construct_bin, VoiceRecvBin construction failed !!");
			return FALSE;
		} else {
			gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpRecordAudioRecvBin->m_bin), NULL); 
		}
	}  
	return TRUE;
}

GSocket * HmpRecordAudioRecvChannel::get_recv_socket()
{
	GSocket *socket = NULL;
	if(m_pHmpRecordAudioRecvBin) {
		socket = m_pHmpRecordAudioRecvBin->m_recvSocket; 
	}
	return socket;
}

void HmpRecordAudioRecvChannel::set_recv_socket(GSocket *socket)
{
	if(m_pHmpRecordAudioRecvBin) {
		m_pHmpRecordAudioRecvBin->m_recvSocket = socket; 
	}
}

gint HmpRecordAudioRecvChannel::get_recv_port()
{
	gint recvPort = 0;
	if(m_pHmpRecordAudioRecvBin) {
		recvPort = m_pHmpRecordAudioRecvBin->m_recvPort; 
	}
	return recvPort;
}

void HmpRecordAudioRecvChannel::set_recv_port(gint port)
{
	if(m_pHmpRecordAudioRecvBin) {
		m_pHmpRecordAudioRecvBin->m_recvPort = port; 
	}
}

gboolean HmpRecordAudioRecvChannel::register_rtpbin_callback(gpointer data)
{
	if(m_pHmpRecordAudioRecvBin) {
		return m_pHmpRecordAudioRecvBin->register_rtpbin_callback(data);
	}
	return FALSE;
}


gboolean HmpRecordAudioRecvChannel::register_audio_buffer_probe_callBack (gpointer data,
                                                                     GstElement *element)
{
  if(m_pHmpRecordAudioRecvBin){
    return m_pHmpRecordAudioRecvBin->register_audio_buffer_probe_callBack(data, element);
 }
 return FALSE;
}
/*  HmpRecordAudioRecvChannel derived class - End
*/

/* HmpRecordVideoRecvBin class - Start
*/
HmpRecordVideoRecvBin::HmpRecordVideoRecvBin(string clientId, string binName, gint recvPort, GSocket *recvSock) 
	: HmpGstBin(clientId, binName), m_udpSrc(NULL), m_queue(NULL), m_rtpBin(NULL),
	m_recvPort(recvPort), m_recvSocket(recvSock), m_streamCount(0),ref_ssrc(0)
{
	XGLOG_INFO( "HmpRecordVideoRecvBin::HmpRecordVideoRecvBin construction start");
}

HmpRecordVideoRecvBin::~HmpRecordVideoRecvBin()
{
	if(m_recvSocket) {
		g_socket_close(m_recvSocket, NULL);
		g_object_unref(G_OBJECT(m_recvSocket));
		m_recvSocket = NULL;
		m_recvPort = 0;
	}
	XGLOG_INFO("HmpRecordVideoRecvBin destructed");
}

gboolean HmpRecordVideoRecvBin::construct()
{
	//create udpsrc element and bind socket
	if(!create_udpsrc()) {
		XGLOG_WARN("Creating udpsrc failed !");
		goto return_on_fail;
	}

	//create rtpbin element
	if(!create_rtpbin()) {
		XGLOG_WARN("Creating rtpbin failed !");
		goto return_on_fail;
	}

	gst_bin_add_many(m_bin, m_udpSrc, m_rtpBin, NULL);
	if(FALSE == gst_element_link_many(m_udpSrc, m_rtpBin, NULL)) {
		XGLOG_WARN("Linking udpsrc and rtpbin elements failed !");
		goto return_on_fail;
	}

	XGLOG_INFO("HmpRecordVideoRecvBin constructed successfully.");
	return TRUE;

return_on_fail:
	XGLOG_ERROR("HmpRecordVideoRecvBin construction failed !");
	return FALSE;
}

gboolean HmpRecordVideoRecvBin::create_udpsrc()
{
	XGLOG_INFO( "create_udpsrc start");
	if(!m_udpSrc) {
		m_udpSrc = gst_element_factory_make("udpsrc", NULL);
		if(m_udpSrc) {
			if(m_recvSocket) {
				g_object_set(G_OBJECT(m_udpSrc), "socket", m_recvSocket, NULL);
			} else {
				XGLOG_ERROR( "create_udpsrc failed due to invalid udp socket !!"); 
				return FALSE;
			}

			//GstCaps* caps = get_caps();
			GstCaps* caps = gst_caps_from_string("application/x-rtp");
			//GstCaps* caps = gst_caps_from_string("application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)VP8, payload=(int)96");
			if(caps) {
				g_object_set(G_OBJECT(m_udpSrc), "caps", caps, NULL);
				gst_caps_unref(caps);
			} else {
				XGLOG_ERROR( "create_udpsrc setting caps for udpsrc failed !!"); 
				return FALSE;
			}
			g_object_set(G_OBJECT(m_udpSrc), "timeout", 0, "reuse", TRUE, "close-socket", FALSE, NULL);
		} else {
			XGLOG_ERROR( "create_udpsrc gst_element_make udpsrc failed !!");
			return FALSE;
		}
	}
	return TRUE;
}

gboolean HmpRecordVideoRecvBin::create_rtpbin()
{
	XGLOG_INFO( "HmpRecordVideoRecvBin::create_rtpbin start");
	guint latency = 200;
	m_rtpBin = gst_element_factory_make("rtpbin", NULL);
	if(!m_rtpBin) {
		XGLOG_ERROR( "HmpRecordVideoRecvBin::create_rtpbin gst_element_make rtpbin failed !!");
		return FALSE;
	}

	g_object_set(G_OBJECT(m_rtpBin),"autoremove", FALSE, NULL);
	g_object_set(G_OBJECT(m_rtpBin), "latency", latency, NULL);
	return TRUE;
}

gboolean HmpRecordVideoRecvBin::register_rtpbin_callback(gpointer data)
{
	if(m_rtpBin) {
		g_signal_connect(G_OBJECT(m_rtpBin), "pad-added", G_CALLBACK(&on_pad_added), data);
		//g_signal_connect(G_OBJECT(m_rtpBin), "pad-removed", G_CALLBACK(&on_pad_removed), data);
		g_signal_connect(G_OBJECT(m_rtpBin), "request-pt-map", G_CALLBACK(&on_request_pt_map), data);
		return TRUE;
	}
	return FALSE;
}

GstCaps *HmpRecordVideoRecvBin::on_request_pt_map(GstElement *rtpBin, guint session, guint pt, gpointer data)
{
	XGLOG_INFO( "HmpRecordVideoRecvBin::on_request_pt_map triggered for pt %d", pt);

	GstCaps * caps = NULL;
	switch(pt) {
		//case 96 ... 127: //for opus dynamic payload
		case 96 ... 127:
			{
			  #if VP8_VIDEO_CODEC
				caps = gst_caps_new_simple("application/x-rtp",
						"media",G_TYPE_STRING,"video",
						"clock-rate",G_TYPE_INT,90000,
						"encoding-name",G_TYPE_STRING,"VP8",
						"payload",G_TYPE_INT,pt,
						NULL);
			  #elif VP9_VIDEO_CODEC
				caps = gst_caps_new_simple("application/x-rtp",
						"media",G_TYPE_STRING,"video",
						"clock-rate",G_TYPE_INT,90000,
						"encoding-name",G_TYPE_STRING,"VP9",
						"payload",G_TYPE_INT,pt,
	   				NULL);
        #endif
				break;
			}
		default:
			XGLOG_INFO("HmpRecordVideoRecvBin::on_request_pt_map: unsupported payload format '%d'", pt);
			break;
		}
	return caps;
}


void HmpRecordVideoRecvBin::on_pad_added(GstElement *rtpBin, GstPad *pad, gpointer data)
{
	gchar *padName = NULL;
	GstPadDirection padDir = GST_PAD_UNKNOWN; 
	guint sn, ssrc, pt = 0;
	static int ref_ssrc_temp = 0;
	HmpRecordVideoRecvChannel *channel = NULL;
	padName = gst_pad_get_name(pad);
	sscanf(padName, "recv_rtp_src_%u_%u_%u", &sn, &ssrc, &pt); //sample pad name "recv_rtp_src_0_4239506697_96"
	XGLOG_INFO("HmpRecordVideoRecvBin::on_pad_added triggered for pad: '%s'", padName);
	ref_ssrc_temp = ssrc;
	Client* client = (Client*)data;
	if(!client){
	  XGLOG_WARN("Invalid client pointer received for payload '%u'", pt);
	  goto return_on_fail;
	}
	channel = client->m_pHmpRecordVideoRecvChannel;
	// cppcheck-suppress nullPointerRedundantCheck
	if(!channel) {
		XGLOG_WARN("Invalid HmpRecordVideoRecvChannel pointer received for payload '%u'", pt);
		goto return_on_fail;
	}

	channel->m_pHmpRecordVideoRecvBin->ref_ssrc = ref_ssrc_temp;
	padDir = gst_pad_get_direction(pad);
	switch(padDir) {
		case GST_PAD_SRC:
			{
				//get new proxysrc pad from compositor to relay new video stream
				GstElement *proxySrc = get_new_compositor_pad(channel);
				if(!proxySrc) {
					XGLOG_WARN("Getting new compositor pad for video ssrc '%u' and payload '%u' failed !", ssrc, pt);
					goto return_on_fail;
				}

				//create depay decorder elements
				GstElement *proxySink = attach_depay_decoder(pad, pt, ssrc, channel, client);
				if(!proxySink) {
					XGLOG_WARN("attaching depay and decoder elements for video ssrc '%u' and payload '%u' failed !", ssrc, pt);
					if(proxySrc) gst_object_unref(GST_OBJECT(proxySrc));
					goto return_on_fail;
				}

				//attach new porxysink and proxysrc elements
				g_object_set(proxySrc, "proxysink", proxySink, NULL);
				client->proxy_pairs[proxySink] = proxySrc;
				break;
			}
		case GST_PAD_SINK: 
			{
				goto return_on_fail;
			}
		case GST_PAD_UNKNOWN:
		default:
			{
				goto return_on_fail;
			}
	}

	channel->draw_dot_file(); 

	g_free(padName);
	XGLOG_INFO("handling on_pad_added event for payload '%u' success!", pt);
	return;

return_on_fail:
	g_free(padName);
	XGLOG_ERROR("handling on_pad_added event for payload '%u' failed!", pt);
  return;
}

GstElement * HmpRecordVideoRecvBin::get_new_compositor_pad(HmpRecordVideoRecvChannel *channel)
{
	XGLOG_INFO("Tyring to relay new video stream to compositor");
	HmpVideoCompositorChannel *compositorChannel = channel->m_pHmpCompositorChannel;
	if(!compositorChannel) {
		XGLOG_ERROR("Invalid compositor channel in HmpRecordVideoRecv channel.");
		return NULL;
	}
	return compositorChannel->get_new_compositor_pad(channel->m_pHmpRecordVideoRecvBin->ref_ssrc);
}

GstElement * HmpRecordVideoRecvBin::attach_depay_decoder(GstPad *pad, guint &pt, guint &ssrc, 
		HmpRecordVideoRecvChannel *channel, gpointer data)
{
	XGLOG_INFO("creating depay and decoder elements to handle payload '%u'", pt);
	GstElement *depay, *decoder, *proxySink, *eleQue;
	depay = NULL; decoder = NULL; proxySink = NULL; eleQue = NULL;
	GstPad *sinkpad = NULL;
	HmpRecordVideoRecvBin *recvBin = channel->m_pHmpRecordVideoRecvBin;
	GstElement *video_rate, *video_convert, *video_scale, *caps_filter;
	video_rate = NULL; video_convert = NULL; video_scale = NULL; caps_filter = NULL;

	eleQue = gst_element_factory_make("queue",NULL);

	GstCaps *caps = gst_pad_get_current_caps(pad);
	GstStructure *structure = gst_caps_get_structure(caps, 0);
	const gchar *encoding_name = gst_structure_get_string(structure, "encoding-name");
  gst_caps_unref(caps);

	if(!encoding_name){
		g_print("Encoding name not found in caps.\n");
	}
  
  video_rate = gst_element_factory_make("videorate", NULL);
  video_convert = gst_element_factory_make("videoconvert", NULL);
 	video_scale = gst_element_factory_make("videoscale", NULL);
	caps_filter = gst_element_factory_make("capsfilter",NULL);
	GstCaps *filter_caps = gst_caps_from_string(CAPS_FILTER_VID_RAW);
	g_object_set(G_OBJECT(caps_filter), "caps",filter_caps, NULL);
	gst_caps_unref(filter_caps);


	if(!video_rate && !video_convert && !video_scale && !caps_filter) {
		XGLOG_ERROR("Invalid videoconvert, videoscale and capsfilter element.");
		goto return_on_fail;
	}

	switch(pt)
	{
		case 96 ... 127:
			{
				if(!strcmp(encoding_name,"VP8")){
		      g_print("Encoding name is: %s\n", encoding_name);
					depay = gst_element_factory_make("rtpvp8depay", NULL);
					g_object_set(G_OBJECT(depay), "wait-for-keyframe",true, NULL);
				  decoder = gst_element_factory_make("vp8dec", NULL);
				  //Discard frames marked as corrupted instead of outputting them
				  g_object_set(G_OBJECT(decoder), "discard-corrupted-frames",true, NULL);
				}else if(!strcmp(encoding_name,"VP9")){
		      g_print("Encoding name is: %s\n", encoding_name);
				  depay = gst_element_factory_make("rtpvp9depay", NULL);
					g_object_set(G_OBJECT(depay), "max-reorder",200, NULL);

					string vp9dec_name = "vp9dec_"+to_string(ssrc);
					decoder = gst_element_factory_make("vp9dec" , vp9dec_name.c_str());
					g_object_set(G_OBJECT(decoder), "discard-corrupted-frames",true, NULL);
					g_object_set(G_OBJECT(decoder), "max-errors", -1, "min-force-key-unit-interval",
                                                               KEYFRAME_INTERVAL_NS, NULL);

					GstPad *decoder_sink_pad = gst_element_get_static_pad(decoder, "sink");
					gulong probe_id = gst_pad_add_probe(decoder_sink_pad, GST_PAD_PROBE_TYPE_EVENT_UPSTREAM,
                                                            vp9dec_sink_buffer_probe, data, NULL);

					g_object_set_data(G_OBJECT(decoder_sink_pad), "pad_probe_id", (gpointer)(guintptr)probe_id);
					gst_object_unref(decoder_sink_pad);
				}else{
					g_print("Encoding name is undefined : [%s]\n", encoding_name);
				}
					break;
  		}
		default:
			{
				XGLOG_WARN("ignoring not supporting payload '%u' !", pt);
				return NULL;
			}
	}

	if(!depay && !decoder) {
		XGLOG_ERROR("Invalid depay or decoder element.");
		goto return_on_fail;
	}

	proxySink = gst_element_factory_make("proxysink", NULL);
	if(!proxySink) {
		XGLOG_ERROR("Invalid proxysink element.");
		goto return_on_fail;
	}
	
	//add decoder elements
	gst_bin_add_many(recvBin->m_bin, eleQue, depay, decoder, video_convert, video_scale, video_rate, caps_filter, proxySink, NULL);
	if(FALSE == gst_element_link_many(eleQue, depay, decoder, video_convert, video_scale, video_rate, caps_filter, proxySink, NULL)) {
		gst_bin_remove_many(recvBin->m_bin, eleQue, depay, decoder, video_convert, video_scale, video_rate, caps_filter, proxySink, NULL);
		XGLOG_WARN("Linking depay, decoder, video_rate, video_convert, video_scale, caps_filter, proxySink elements failed!");
		XGLOG_ERROR("Relaying new video stream to compoistor channel for payload '%u' failed!", pt);
		return NULL;
	}

	//linking decoder queue with rtpbin dynamic src pad
	sinkpad = gst_element_get_static_pad(eleQue, "sink");
	gst_pad_link(pad, sinkpad);
	gst_object_unref(GST_OBJECT(sinkpad));

	//sync newly added elements with parent
	gst_element_sync_state_with_parent(eleQue);
	gst_element_sync_state_with_parent(proxySink);
	gst_element_sync_state_with_parent(decoder);
	gst_element_sync_state_with_parent(depay);
	gst_element_sync_state_with_parent(video_scale);
	gst_element_sync_state_with_parent(caps_filter);
	gst_element_sync_state_with_parent(video_convert);
  gst_element_sync_state_with_parent(video_rate);

	XGLOG_INFO("Attaching depayloader, decoder and proxySink in HmpRecordVideoRecv channel to handle payload '%u' success!", pt);
	return proxySink;

return_on_fail:
	if(eleQue) gst_object_unref(GST_OBJECT(eleQue));
	if(depay) gst_object_unref(GST_OBJECT(depay));
	if(decoder) gst_object_unref(GST_OBJECT(decoder));
	if(proxySink) gst_object_unref(GST_OBJECT(proxySink));
	if(video_scale) gst_object_unref(GST_OBJECT(video_scale));
	if(caps_filter) gst_object_unref(GST_OBJECT(caps_filter));
	if(video_convert) gst_object_unref(GST_OBJECT(video_convert));
  if(video_rate) gst_object_unref(GST_OBJECT(video_rate));

	XGLOG_ERROR("Attaching depayloader, decoder and proxySink in HmpRecordVideoRecv channel to handle payload '%u' failed!", pt);
	return NULL;
}

gboolean HmpVideoCompositorBin::register_video_buffer_probe_callBack (gpointer data, GstElement *element)
{
 gulong probeId = 0;
 GstPad *srcPad = NULL;
 if(element){
   srcPad = gst_element_get_static_pad (element, "src");
   if (srcPad){
     probeId = gst_pad_add_probe (srcPad,(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_PUSH ),
                                handle_VideoPadProbeCallback, data, NULL);
     XGLOG_INFO("register_video_buffer_probe_callBack generated buffer probe_Id: %lu ", probeId);
     gst_object_unref(srcPad);
     return TRUE;
   } else {
     XGLOG_ERROR ("Failed to get src pad for capsfilter element");
   }
 }
 gst_object_unref(srcPad);
 return FALSE;
}

GstPadProbeReturn HmpVideoCompositorBin::handle_VideoPadProbeCallback (GstPad * pad,
                                                         GstPadProbeInfo * ProbeInfo,
                                                         gpointer data)
{
  Client *client = (Client*)data;
  GstBuffer *buffer = NULL;
  GstMapInfo info;
  buffer = gst_pad_probe_info_get_buffer (ProbeInfo);
  HmpAudioVideoMuxRecorderBin *pHmpAudioVideoMuxRecorderBin = NULL;
  pHmpAudioVideoMuxRecorderBin = client->m_pHmpAudioVideoMuxRecorderChannel->m_pHmpAudioVideoMuxRecorderBin;
  GstElement* appsrc = pHmpAudioVideoMuxRecorderBin->m_videoSrc;
  GstFlowReturn ret = GST_FLOW_ERROR;
  GstClockTime now = 0;

  if(pHmpAudioVideoMuxRecorderBin->is_pipeline_paused){
    return GST_PAD_PROBE_DROP;
  }

  if (!buffer){
    XGLOG_ERROR ("Buffer is empty ");
    goto return_from_func;
  }

  now = gst_element_get_current_running_time(appsrc);
  GST_BUFFER_PTS(buffer) = now;
  GST_BUFFER_DTS(buffer) = now;

  if(pHmpAudioVideoMuxRecorderBin->set_videocaps && appsrc){
    GstCaps *video_caps = gst_pad_get_current_caps(pad);
    gst_app_src_set_caps(GST_APP_SRC(appsrc), video_caps);
    XGLOG_INFO("Successfully set the video Appsrc caps!!!");
    gst_caps_unref(video_caps);
    pHmpAudioVideoMuxRecorderBin->set_videocaps = false;
  }

  ret = gst_app_src_push_buffer(GST_APP_SRC(appsrc),buffer);

  if(GST_FLOW_OK != ret){
    XGLOG_ERROR("Failed to push buffer to the video Appsc!!!");
  }
return_from_func:
  return GST_PAD_PROBE_HANDLED ;
}
/* HmpRecordVideoRecvBin class - End
*/

/* HmpRecordVideoRecvChannel derived class - Start
*/
HmpRecordVideoRecvChannel::HmpRecordVideoRecvChannel(string clientId, string pipelineName, HmpChannelType pipelineType) :
	HmpGstChannel(clientId, pipelineName, pipelineType) , m_pHmpRecordVideoRecvBin(NULL), m_pHmpCompositorChannel(NULL)
{
	XGLOG_INFO( "HmpRecordVideoRecvChannel::HmpRecordVideoRecvChannel construction start");
}

HmpRecordVideoRecvChannel::~HmpRecordVideoRecvChannel()
{
	XGLOG_INFO( "HmpRecordVideoRecvChannel::~HmpRecordVideoRecvChannel destruction start");
	if(m_pHmpRecordVideoRecvBin) {
		delete m_pHmpRecordVideoRecvBin;
		m_pHmpRecordVideoRecvBin = NULL;
	}
	if(m_pHmpCompositorChannel) {
		delete m_pHmpCompositorChannel;
		m_pHmpCompositorChannel = NULL;
	}
}

gboolean HmpRecordVideoRecvChannel::construct_bin(string binName, gint recvPort, GSocket *recvSock, string Browser_Type)
{
	XGLOG_INFO( "HmpRecordVideoRecvChannel::construct_bin %s start", binName.c_str());
	//construct video compositor channel
	if(!m_pHmpCompositorChannel) {
		m_pHmpCompositorChannel = new HmpVideoCompositorChannel(m_clientId, "VideoCompositorChannel_", EN_PIPELINE_VIDEO_COMPOSITOR);
		if(!m_pHmpCompositorChannel->construct_bin("VideoCompositorBin_",Browser_Type)) {
			XGLOG_ERROR( "VideoCompositorBin construction failed for client: '%s'", m_clientId.c_str());
			return FALSE;
		}
	}
	//m_pHmpCompositorChannel->draw_dot_file();
	//m_pHmpCompositorChannel->start();

	//construct video receive bin
	if(!m_pHmpRecordVideoRecvBin) {
		m_pHmpRecordVideoRecvBin = new HmpRecordVideoRecvBin(m_clientId, binName, recvPort, recvSock); 
		if(!m_pHmpRecordVideoRecvBin->construct()) {
			XGLOG_ERROR( "construct_bin, VideoRecvBin construction failed !!");
			return FALSE;
		} else {
			gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpRecordVideoRecvBin->m_bin), NULL); 
		}
	}  

	return TRUE;
}

GSocket * HmpRecordVideoRecvChannel::get_recv_socket()
{
	GSocket *socket = NULL;
	if(m_pHmpRecordVideoRecvBin) {
		socket = m_pHmpRecordVideoRecvBin->m_recvSocket; 
	}
	return socket;
}

void HmpRecordVideoRecvChannel::set_recv_socket(GSocket *socket)
{
	if(m_pHmpRecordVideoRecvBin) {
		m_pHmpRecordVideoRecvBin->m_recvSocket = socket; 
	}
}

gint HmpRecordVideoRecvChannel::get_recv_port()
{
	gint recvPort = 0;
	if(m_pHmpRecordVideoRecvBin) {
		recvPort = m_pHmpRecordVideoRecvBin->m_recvPort; 
	}
	return recvPort;
}

void HmpRecordVideoRecvChannel::set_recv_port(gint port)
{
	if(m_pHmpRecordVideoRecvBin) {
		m_pHmpRecordVideoRecvBin->m_recvPort = port; 
	}
}

gboolean HmpRecordVideoRecvChannel::register_rtpbin_callback(gpointer data)
{
	if(m_pHmpRecordVideoRecvBin) {
		return m_pHmpRecordVideoRecvBin->register_rtpbin_callback(data);
	}
	return FALSE;
}
/*  HmpRecordVideoRecvChannel derived class - End
*/

/*  HmpVideoCompositorBin derived class - Start
*/
HmpVideoCompositorBin::HmpVideoCompositorBin(string clientId, string binName,string browser_type) 
	: HmpGstBin(clientId, binName), m_compositor(NULL), m_queue(NULL), m_videoEnc(NULL), Enc_filter(NULL),m_videoconvert(NULL),m_videoscale(NULL), m_streamCount(0)
{
	Browser_Type = browser_type;
	XGLOG_INFO( "HmpVideoCompositorBin::HmpVideoCompositorBin construction start");
}

HmpVideoCompositorBin::~HmpVideoCompositorBin()
{
	XGLOG_INFO("HmpVideoCompositorBin destructed");
}

gboolean HmpVideoCompositorBin::construct()
{
	if(!create_elements()) {
		XGLOG_ERROR( "construct HmpVideoCompositorBin, creating elements failed !");
		return FALSE;
	}

	if(!add_and_link_elements()) {
		XGLOG_ERROR( "construct HmpVideoCompositorBin, add and linking elements failed !");
		return FALSE;
	}
	return TRUE;
}

gboolean HmpVideoCompositorBin::create_elements()
{
	XGLOG_INFO("HmpVideoCompositorBin::create_elements start");
	m_compositor = gst_element_factory_make("compositor", NULL);
	m_videoconvert = gst_element_factory_make("videoconvert", NULL);
	m_videoscale = gst_element_factory_make("videoscale", NULL);
	g_object_set(G_OBJECT(m_compositor), "ignore-inactive-pads", FALSE, NULL);
	g_object_set(G_OBJECT(m_compositor), "max-threads", 6, NULL);
	g_object_set(G_OBJECT(m_compositor), "background", 1, NULL);

	m_queue = gst_element_factory_make("queue", NULL);
  g_object_set(G_OBJECT(m_queue), "max-size-bytes", 12582912, NULL);
  g_object_set(G_OBJECT(m_queue), "max-size-buffers", 50, NULL);

	Enc_filter = gst_element_factory_make("capsfilter", NULL);
	GstCaps *filter_caps = gst_caps_from_string(CAPS_FILTER_VID_RAW_ENCODE);
	g_object_set(G_OBJECT(Enc_filter), "caps", filter_caps, NULL);

	XGLOG_INFO("Mac/IOS HmpVideoCompositorBin::create_elements Browser_Type [%s] ",Browser_Type.c_str());
	m_videoEnc = gst_element_factory_make("x264enc", NULL);
	g_object_set (G_OBJECT(m_videoEnc) , "bitrate", 800, NULL);
	g_object_set (G_OBJECT(m_videoEnc) , "threads", 2, NULL);
	g_object_set (G_OBJECT(m_videoEnc) , "speed-preset", 2, NULL);//encoding process will be in "superfast" mode
	g_object_set (G_OBJECT(m_videoEnc) , "tune", 0x00000004, NULL);//zero-latency mode

	if(!m_compositor || !m_queue || !Enc_filter || !m_videoconvert || !m_videoscale || !m_videoEnc) {
		XGLOG_INFO("creating elements for HmpVideoCompositorBin failed for client: '%s'", m_clientId.c_str());
		return FALSE;
	}
  gst_caps_unref(filter_caps);
	return TRUE;
}

gboolean HmpVideoCompositorBin::add_and_link_elements()
{
	if (!m_bin || !m_compositor || !m_queue || !Enc_filter || !m_videoconvert || !m_videoscale || !m_videoEnc)
	{
		XGLOG_ERROR("construct HmpVideoCompositorBin, creating elements failed !!");
		return FALSE;
	}

	// if(Browser_Type.empty()){
	// 	return FALSE;
	// }

	if(!Browser_Type.empty() && (strstr(Browser_Type.c_str(),"Mac") || strstr(Browser_Type.c_str(),"IOS"))){
		gst_bin_add_many(m_bin, m_compositor, m_queue, m_videoconvert, m_videoscale, Enc_filter, m_videoEnc, NULL);
		if (gst_element_link_many(m_compositor, m_queue, m_videoconvert, m_videoscale, Enc_filter, m_videoEnc, NULL) == TRUE)
		{
			XGLOG_INFO("HmpVideoCompositorBin add_and_link_elements success");
		}
		else
		{
			XGLOG_ERROR("HmpVideoCompositorBin add_and_link_elements failed!");
			return FALSE;
		}
	}else{
		gst_bin_add_many(m_bin, m_compositor, m_queue, m_videoconvert, m_videoscale, Enc_filter, m_videoEnc, NULL);
		if (gst_element_link_many(m_compositor, m_queue, m_videoconvert, m_videoscale, Enc_filter, m_videoEnc, NULL) == TRUE)
		{
			XGLOG_INFO("HmpVideoCompositorBin add_and_link_elements success");
		}
		else
		{
			XGLOG_ERROR("HmpVideoCompositorBin add_and_link_elements failed!");
			return FALSE;
		}
	}
	return TRUE;
}

GstPad * HmpVideoCompositorBin::get_dynamically_configured_pad()
{
	++m_streamCount;  
	
  GstPad *newPad = gst_element_get_request_pad(m_compositor, "sink_%u");
	if(!newPad) {
		XGLOG_ERROR("Getting new dynamically configured sink pad from compositor for client: '%s' failed !!", m_clientId.c_str());
		return NULL;	
	}

	//update common video sink properties first
	g_object_set(newPad, "sizing-policy", 1, NULL); //1='keep-aspect-ratio'
	guint xPos = 0;
	guint yPos = 0;
	guint frameNewWidth = 0, frameNewHeight = 0;
  GstIterator *it = gst_element_iterate_sink_pads(m_compositor);
	guint count = m_compositor->numsinkpads;
	GValue item = G_VALUE_INIT;
	gboolean done = FALSE;

	// guint count = m_streamCount;
	int row, col = ceil(sqrt(count));
	row = round(sqrt(count));
	if(col > 0)
	  frameNewWidth = DEFAULT_RECORD_SCREEN_WIDTH/col;
	if(row > 0)
	  frameNewHeight = DEFAULT_RECORD_SCREEN_HEIGHT/row; //on every new stream, each existing frame hight, width will be callculated
	for(int i=0; i<row; i++,yPos += frameNewHeight){
    if(i == row-1 && m_compositor->numsinkpads % col != 0){
      xPos = (frameNewWidth/2)*(col-m_compositor->numsinkpads%col);
    }
    else
      xPos = 0;
    for(int j=0; j<col && count--; j++, xPos+=frameNewWidth){
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
            XGLOG_INFO("Reconfiguring '%u' compositor sink pad at x:y pos '%u:%u'", count, xPos, yPos);
          break;
        }
      case GST_ITERATOR_ERROR:
        {
          done = TRUE;
          XGLOG_ERROR("Recofiguring compositor sink pads for client: '%s' failed !!", m_clientId.c_str());
          break;
        }
      case GST_ITERATOR_DONE:
        {
          XGLOG_INFO("Reconfigured all compositor sink pad at frame size '%u:%u'", frameNewWidth, frameNewHeight);
          done = TRUE;
          break;
        }
      }
    }
  }
	draw_dot_file();

	XGLOG_INFO("'%u' video frames handled by compositor for client: '%s'", m_compositor->numsinkpads, m_clientId.c_str());
  gst_iterator_free(it);
	return newPad;
}

GstElement * HmpVideoCompositorBin::get_new_compositor_pad(unsigned int ref_ssrc)
{
	string proxysrc_name = "proxysrc_" + to_string(ref_ssrc);
	if(m_compositor) {
		GstElement *proxySrc = gst_element_factory_make("proxysrc", proxysrc_name.c_str());
		//TODO: add new proxysrc element in compositor bin
		gst_bin_add_many(m_bin, proxySrc, NULL);
		GstPad *sinkPad = get_dynamically_configured_pad();
		if(sinkPad) {
			//link proxysrc with compositor new request pad
			GstPad *srcPad = gst_element_get_static_pad(proxySrc, "src");
			GstPadLinkReturn lres = GST_PAD_LINK_REFUSED;
			if(srcPad != NULL)
			  lres = gst_pad_link(srcPad, sinkPad);
			if(lres != GST_PAD_LINK_OK) {
				XGLOG_ERROR("Linking new proxysrc with new compositor pad for client: '%s' failed !!", m_clientId.c_str());
				g_object_unref(GST_OBJECT(proxySrc));
				return NULL;
			}
			XGLOG_INFO("Created and linked new proxysrc with new compositor pad for client: '%s' success !!", m_clientId.c_str());
			//sync newly added elements with parent
			gst_element_sync_state_with_parent(proxySrc);
	                g_object_unref(GST_OBJECT(srcPad));

			return proxySrc;
		}
	        g_object_unref(GST_OBJECT(sinkPad));
	}
	XGLOG_ERROR("attach new video stream in compositor bin for client: '%s' failed !!", m_clientId.c_str());
	return NULL;
}

/*  HmpVideoCompositorBin derived class - End
*/

/*  HmpVideoCompositorChannel derived class - Start
*/
HmpVideoCompositorChannel::HmpVideoCompositorChannel(string clientId, string pipelineName, HmpChannelType pipelineType) :
	HmpGstChannel(clientId, pipelineName, pipelineType), m_pHmpVideoCompositorBin(NULL)
{
}

HmpVideoCompositorChannel::~HmpVideoCompositorChannel()
{
	XGLOG_INFO("HmpVideoCompostorChannel::~HmpVideoCompositorChannel destruction start");
	if(m_pHmpVideoCompositorBin) {
		delete m_pHmpVideoCompositorBin;
	}
}

gboolean HmpVideoCompositorChannel::construct_bin(string binName,string Browser_Type)
{
	XGLOG_INFO("HmpVideoCompositorChannel::construct_bin %s start", binName.c_str());
	if(!m_pHmpVideoCompositorBin) {
		m_pHmpVideoCompositorBin = new HmpVideoCompositorBin(m_clientId, binName, Browser_Type);
		if(!m_pHmpVideoCompositorBin->construct()) {
			XGLOG_ERROR( "construct_bin, HmpVideoCompositor construction failed !!");
			return FALSE;
		} else {
			gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpVideoCompositorBin->m_bin), NULL);
		}
	}
	return TRUE;
}

GstElement * HmpVideoCompositorChannel::get_new_compositor_pad(unsigned int ref_ssrc)
{
	GstElement *proxySrc = m_pHmpVideoCompositorBin->get_new_compositor_pad(ref_ssrc);
	draw_dot_file();
	return proxySrc;
}

gboolean HmpVideoCompositorChannel::register_video_buffer_probe_callBack (gpointer data,
                                                                     GstElement *element)
{
  if(m_pHmpVideoCompositorBin){
    return m_pHmpVideoCompositorBin->register_video_buffer_probe_callBack(data, element);
 }
 return FALSE;
}

/*  HmpVideoCompositorChannel derived class - End
*/

/*  HmpAudioVideoMuxRecorderBin derived class - Start
*/
HmpAudioVideoMuxRecorderBin::HmpAudioVideoMuxRecorderBin(string clientId, string binName,string browser_type)
	: HmpGstBin(clientId, binName), m_audioSrc(NULL), m_videoSrc(NULL), m_webmMux(NULL), m_fileSink(NULL),
  set_audiocaps(true), set_videocaps(true), is_pipeline_paused(false)
{
	Browser_Type = browser_type;
	XGLOG_INFO( "HmpAudioVideoMuxRecorderBin::HmpAudioVideoMuxRecorderBin construction start");
}

HmpAudioVideoMuxRecorderBin::~HmpAudioVideoMuxRecorderBin()
{
	XGLOG_INFO("HmpAudioVideoMuxRecorderBin destructed");
}

gboolean HmpAudioVideoMuxRecorderBin::construct()
{
	if(!create_elements()) {
		XGLOG_ERROR( "construct HmpAudioVideoMuxRecorderBin, creating elements failed !");
		return FALSE;
	}

	if(!add_and_link_elements()) {
		XGLOG_ERROR( "construct HmpAudioVideoMuxRecorderBin, add and linking elements failed !");
		return FALSE;
	}
	return TRUE;
}

gboolean HmpAudioVideoMuxRecorderBin::create_elements()
{
	XGLOG_INFO("HmpAudioVideoMuxRecorderBin::create_elements start");
	m_audioSrc = gst_element_factory_make("appsrc",NULL);

	m_videoSrc = gst_element_factory_make("appsrc",NULL);

	m_webmMux = gst_element_factory_make("mp4mux",NULL);

	XGLOG_INFO("HmpAudioVideoMuxRecorderBin::create_elements Browser_Type: [%s]",Browser_Type.c_str());
	m_fileSink = gst_element_factory_make("filesink",NULL);
	if(!m_audioSrc || !m_videoSrc || !m_webmMux || !m_fileSink) {
		XGLOG_INFO("creating elements for HmpAudioVideoMuxRecorderBin failed for client: '%s'", m_clientId.c_str());
		return FALSE;
	}
  g_object_set(G_OBJECT(m_audioSrc), "is-live", TRUE, "do-timestamp", TRUE, "format", GST_FORMAT_TIME, NULL);
  g_object_set(G_OBJECT(m_videoSrc), "is-live", TRUE, "do-timestamp", TRUE, "format", GST_FORMAT_TIME, NULL);
  g_object_set(G_OBJECT(m_fileSink), "o-sync", TRUE, NULL);

	return TRUE;
}

gboolean HmpAudioVideoMuxRecorderBin::add_and_link_elements()
{
	GstPad *srcpad, *sinkpad = NULL;
	GstPadLinkReturn lres;

	if(!m_bin || !m_audioSrc || !m_videoSrc || !m_webmMux || !m_fileSink)
	{
		XGLOG_ERROR( "construct HmpAudioVideoMuxRecorderBin, creating elements failed !!");
		return FALSE;
	}

	gst_bin_add_many(m_bin, m_audioSrc, m_videoSrc, m_webmMux, m_fileSink, NULL);
	srcpad = gst_element_get_static_pad(m_audioSrc,"src");
	sinkpad = gst_element_get_request_pad(m_webmMux,"audio_%u");
	lres = gst_pad_link(srcpad, sinkpad);
	g_object_unref(GST_OBJECT(srcpad));
	g_object_unref(GST_OBJECT(sinkpad));

	srcpad = NULL;
	sinkpad = NULL; 

	srcpad = gst_element_get_static_pad(m_videoSrc, "src");
	sinkpad = gst_element_get_request_pad(m_webmMux, "video_%u");
	lres = gst_pad_link(srcpad, sinkpad);

	g_object_unref(GST_OBJECT(srcpad));
	g_object_unref(GST_OBJECT(sinkpad));

	if(gst_element_link_many(m_webmMux, m_fileSink, NULL) == TRUE) {
		XGLOG_INFO("HmpAudioVideoMuxRecorderBin add_and_link_elements success");
	} else {
		XGLOG_ERROR( "HmpAudioVideoMuxRecorderBin add_and_link_elements failed!");
		return FALSE;
	}

	return TRUE;
}

/*  HmpAudioVideoMuxRecorderBin derived class - Start
*/

/*  HmpAudioVideoMuxRecorderChannel derived class - Start
*/
HmpAudioVideoMuxRecorderChannel::HmpAudioVideoMuxRecorderChannel(string clientId, string pipelineName, HmpChannelType pipelineType) :
	HmpGstChannel(clientId, pipelineName, pipelineType), m_pHmpAudioVideoMuxRecorderBin(NULL)
{
}

HmpAudioVideoMuxRecorderChannel::~HmpAudioVideoMuxRecorderChannel()
{
	XGLOG_INFO("HmpAudioVideoMuxRecorderChannel::~HmpAudioVideoMuxRecorderChannel destruction start");
	if(m_pHmpAudioVideoMuxRecorderBin) {
		delete m_pHmpAudioVideoMuxRecorderBin;
	}
}

gboolean HmpAudioVideoMuxRecorderChannel::construct_bin(string binName,string Browser_Type)
{
	XGLOG_INFO("HmpAudioVideoMChannel::construct_bin %s start", binName.c_str());
	if(!m_pHmpAudioVideoMuxRecorderBin) {
		m_pHmpAudioVideoMuxRecorderBin = new HmpAudioVideoMuxRecorderBin(m_clientId, binName, Browser_Type);
		if(!m_pHmpAudioVideoMuxRecorderBin->construct()) {
			XGLOG_ERROR( "construct_bin, HmpAudioVideoMixerBin construction failed !!");
			return FALSE;
		} else {
			gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpAudioVideoMuxRecorderBin->m_bin), NULL);
		}
	}
	return TRUE;
}
/*  HmpAudioVideoMuxRecorderChannel derived class - End
*/
