//local include

//ace include
#include <ace/Task.h>
#include <time.h>
#include<mutex>
//self include
#include "xGateHmpGstBin.h"
#include "xGateHmpGstUtil.h"
#include "xGateHmpVP9Parser.h"
#include "xGateHmpGstVideoBin.h"
#include <gst/sdp/sdp.h>
#include <gst/webrtc/webrtc.h>
#include <gst/webrtc/webrtc_fwd.h>
#include <gst/gstbuffer.h>
#include <gst/rtp/gstrtpbuffer.h>
#include <gst/rtp/gstrtcpbuffer.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include "xglog.h"
#include "xGateSFUtil.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#define THISMODULE "VidBin"
#define USE_SEPARATE_THREAD_TO_SEND_DATA
//#define DUMP_RTCP_PKT
/* HmpVideoBin Base class - Start
*/
/*constructor and destuctor */
HmpVideoBin::HmpVideoBin(string binName, HmpVideoBinType binType, unsigned int dynamicCodec = 0) :
  m_binName(binName), m_vbinType(binType), m_bin(NULL),
  m_dynamicCodec(dynamicCodec)
{
  m_bin = GST_BIN(gst_bin_new(binName.c_str()));
}

HmpVideoBin::~HmpVideoBin()
{
  XGLOG_INFO( "~HmpVideoBin destructor called for bin: %s", m_binName.c_str());
}

gboolean HmpVideoBin::draw_dot_file()
{
  if(HAVE_GST_DEBUG_PRINTS && m_bin && !m_binName.empty()) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_bin, GST_DEBUG_GRAPH_SHOW_FULL_PARAMS, m_binName.c_str());
    return TRUE;
  }
  return FALSE;
}

gboolean HmpVideoBin::send_stream_start_event (void *obj,
                                               MgGstEventType eEvent,
                                               gchar *u8StreamId)
{
   GstElement  *pElement = NULL;
   GstPad      *pPad = NULL;
   GstEvent    *pStreamEvent = NULL;

   if (eEvent == STREAM_START_EVENT_PAD) {
      pPad = (GstPad *)obj;
   } else if (eEvent == STREAM_START_EVENT_ELEMENT) {
      pElement = (GstElement *)obj;
   } else {
     return FALSE;
   }
   if (u8StreamId) {
     pStreamEvent = gst_event_new_stream_start (u8StreamId);
     if (!pStreamEvent){
       XGLOG_WARN ("stream start event could not be created for stream_id: %s ",u8StreamId);
       return FALSE;
     }
   }
   if (eEvent == STREAM_START_EVENT_PAD) {
      XGLOG_INFO ("Sending Stream start event: %p to stream-id: %s",
                                    pStreamEvent, u8StreamId);
      if (FALSE == gst_pad_send_event (pPad, pStreamEvent)){
         XGLOG_ERROR ("Failed to send steam start event for stream-id: %s ", u8StreamId);
         return FALSE;
      }
   }
   return TRUE;
}

gboolean HmpVideoBin::send_segment_event (GstPad *sinkPad,
                                          MgGstEventType eEvent)
{
  GstEvent *pSegmentEvent = NULL;
  GstSegment *pSegment = NULL;

 /*Send segment event */
  pSegment = gst_segment_new();
  if (pSegment) {
    if (eEvent == SEGMENT_EVENT_TIME_PAD)
      gst_segment_init( pSegment, GST_FORMAT_TIME);
    else if (eEvent == SEGMENT_EVENT_BYTES_PAD)
      gst_segment_init( pSegment, GST_FORMAT_BYTES);

    pSegment->rate  = 1.0;
    pSegment->start = 0;
    pSegment->stop = -1;

    pSegmentEvent = gst_event_new_segment (pSegment);
    if (pSegmentEvent){
      if (FALSE == gst_pad_send_event (sinkPad, pSegmentEvent)){
        XGLOG_ERROR ("Failed to send SEGMENT event for sinkPad");
        gst_segment_free (pSegment);
        return FALSE;
      }
    }
    gst_segment_free (pSegment);
  }
  return TRUE;
}

/* HmpAudioDecoderBin derived class - start
*/
HmpAudioDecoderBin::HmpAudioDecoderBin(string binName, HmpVideoBinType binType,
                                       HmpCodecType codecType, string decoder_name):
                                       HmpVideoBin(binName,binType,0), m_audioCodecType(codecType)
{
  m_decoder_name = decoder_name;
  XGLOG_INFO( "HmpAudioDecoderBin construction start for webrtc_name: %s ",m_decoder_name.c_str());
  decoder_name.clear();
}

HmpAudioDecoderBin::~HmpAudioDecoderBin()
{
  XGLOG_INFO( "HmpAudioDecoderBin::~HmpAudioDecoderBin destruction ended");
  if(!m_decoder_name.empty()){
    m_decoder_name.clear();
  }
}

gboolean HmpAudioDecoderBin::construct(string decoder_name)
{
  /*create audio decoder */
  if(!create_decoderbin(m_decoder_name)) {
    XGLOG_ERROR( "construct AudioDecoderBin, creating webrtcbin failed !!");
    return FALSE;
  }
  return TRUE;
}

gboolean HmpAudioDecoderBin::create_decoderbin(string decoder_name)
{
  GstPad *pad, *srcpad;
  GstCaps  *caps = NULL;
  GstElement *filesink;
  XGLOG_INFO( "create decoderBin, creating audio_decoderbin");

  m_opusdec = gst_element_factory_make("opusdec", "opusdec_111");
  m_opusdepay = gst_element_factory_make("rtpopusdepay", "opusdepay_111");
  //m_opusdec_queue = gst_element_factory_make("queue", "opusdec_queue");
  m_audioresample = gst_element_factory_make("audioresample", "dec_resampler");
  m_audioconvert = gst_element_factory_make("audioconvert", "dec_audiocovert");
  m_capsfilter = gst_element_factory_make("capsfilter", "capsfilter_opus");
  m_audioLevel = gst_element_factory_make("level", "audio_level");
  //caps = gst_caps_from_string (CAPS_STR_FOR_AUDIOENC);
  caps = gst_caps_from_string (CAPS_STR_FOR_AUDIODEC);
  g_object_set (G_OBJECT(m_capsfilter) ,"caps", caps, NULL);
  g_object_set(G_OBJECT(m_audioresample),"quality",8,NULL);
  gst_bin_add_many (m_bin, m_opusdepay, //m_opusdec_queue,
                   m_opusdec, m_audioresample, m_audioconvert, m_capsfilter, m_audioLevel, NULL);
  gst_element_link_many(//m_opusdec_queue,
                   m_opusdepay, m_opusdec,
                   m_audioconvert, m_audioresample,m_audioLevel, m_capsfilter, NULL);

  if (m_audioLevel){
    //g_object_set (G_OBJECT(m_audioLevel) ,"audio-level-meta", TRUE, NULL);
    g_object_set (G_OBJECT(m_audioLevel) ,"interval", AUDIO_LEVEL_DETECT_DURATION, NULL);

  }
  //pad = gst_element_get_static_pad (m_opusdec_queue, "sink");
  pad = gst_element_get_static_pad (m_opusdepay, "sink");
  m_ghostRtpSinkPad = gst_ghost_pad_new("sink_ghost", pad);
  gst_element_add_pad (GST_ELEMENT(m_bin), m_ghostRtpSinkPad);
  gst_pad_set_active(m_ghostRtpSinkPad, TRUE);
  gst_object_unref (GST_OBJECT (pad));

  return TRUE;
}
/* HmpAudioDecoderBin derived class - end
*/

/* HmpAudioEncoderBin derived class - start
*/
HmpAudioEncoderBin::HmpAudioEncoderBin(string binName, HmpVideoBinType binType,
                                       HmpCodecType codecType, string encoder_name):
     HmpVideoBin(binName,binType,0), m_audioCodecType(codecType)
{
  m_encoder_name = encoder_name;
  XGLOG_INFO( "HmpAudioEncoderBin construction start for AudioEncoder Bin");
  encoder_name.clear();
}

HmpAudioEncoderBin::~HmpAudioEncoderBin()
{
  XGLOG_INFO( "HmpAudioEncoderBin::~HmpAudioEncoderBin destruction ended");
  if(!m_encoder_name.empty()){
    m_encoder_name.clear();
  }
}


gboolean HmpAudioEncoderBin::create_encoderbin(string encoder_name, Client *client)
{
  GstPad *pad, *srcpad ,*sinkpad;
  GstCaps  *caps = NULL;
  GstElement *filesink;
  gulong probeId = 0;
  HmpFrameBin *pHmpFrameBin = NULL;
  m_opusenc = gst_element_factory_make("opusenc", NULL);
  m_opuspay = gst_element_factory_make("rtpopuspay", NULL);
//  m_audioresampler = gst_element_factory_make("audioresample", "enc_resampler");
//  m_audioconvert = gst_element_factory_make("audioconvert", "enc_audioconvert");
  m_opusenc_filter = gst_element_factory_make("capsfilter", "opus_enc_caps_filter");
  //m_opusenc_queue = gst_element_factory_make("queue", "opus_enc_queue");
#ifdef USE_GST_AUDIO_MIXER_PLUGIN
  m_audiomixer = gst_element_factory_make("audiomixer", NULL);
  gst_bin_add_many (m_bin,m_audiomixer, //m_audioconvert, m_audioresampler,
                   m_opusenc_filter, m_opusenc, m_opuspay, //m_opusenc_queue,
                   NULL);
  gst_element_link_many(m_audiomixer, //m_audioconvert, m_audioresampler,
                       //m_opusenc_queue,
                        m_opusenc_filter, m_opusenc, m_opuspay, NULL);
  // configure important parameters of audiomixer like o/p buffer duration,start time*/
  g_object_set (G_OBJECT(m_audiomixer),"output-buffer-duration", 20000000,
                                       "start-time-selection", 1, //GST_AGGREGATOR_START_TIME_SELECTION_FIRST,
                                       "ignore-inactive-pads", TRUE,
                                       "latency", GST_AUDIO_MIXER_LATENCY, // 100ms to reduce silence buffer output
                                       "alignment-threshold", GST_AUDIO_MIXER_THREADSHOLD,
                                       "block-after-latency-limit", FALSE,
                                       NULL);
  srcpad = gst_element_get_static_pad (m_audiomixer, "src");
  probeId = gst_pad_add_probe (srcpad,
                              (GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_PUSH ),
                               handle_GstAudioMixerProbeCallback, client, NULL);

#else
  gst_bin_add_many (m_bin, //m_audioconvert, m_audioresampler,
                   m_opusenc_filter, //m_opusenc_queue,
                   m_opusenc, m_opuspay,
                   NULL);
  gst_element_link_many(
                       m_opusenc_filter,
                      // m_opusenc_queue ,
                       m_opusenc, m_opuspay, NULL);

#endif
  caps = gst_caps_from_string (CAPS_STR_FOR_AUDIOENC);
  g_object_set (G_OBJECT (m_opusenc_filter), "caps", caps, NULL);

   g_object_set (G_OBJECT(m_opusenc) ,"bitrate", 20000,
                                     "frame-size", 20,
                                     //"inband-fec", TRUE,
                                     "complexity", 5,
                                      NULL);
  if (client->m_FrameChannel)
     pHmpFrameBin = client->m_FrameChannel->m_pHmpFrameBin;

  bDtxEnabled = FALSE;
  if (pHmpFrameBin){
    if (TRUE == check_dtx_enabled (pHmpFrameBin->m_remoteSdpStr))
    {
      XGLOG_INFO ("usedtx is enabled .. configuring the encoder with dtx=1");
      g_object_set (G_OBJECT(m_opusenc) ,"dtx", TRUE, NULL);
      g_object_set (G_OBJECT (m_opuspay) , "dtx" , TRUE, NULL);
      bDtxEnabled = TRUE;
    }
  }

  srcpad = gst_element_get_static_pad (m_opuspay, "src");
#ifdef RECORD_ENABLED
  XGLOG_INFO ("Recorder and Audio MCU is enabled. Registering buffer callback");
  probeId = gst_pad_add_probe (srcpad,
                              (GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_PUSH ),
                               handle_EncodedAudioProbeCallback, client, NULL);
#endif
  m_ghostRtpSrcPad = gst_ghost_pad_new("src_ghost_1", srcpad);
  gst_element_add_pad (GST_ELEMENT(m_bin), m_ghostRtpSrcPad);
  gst_pad_set_active(m_ghostRtpSrcPad, TRUE);
  gst_object_unref (GST_OBJECT (srcpad));

  g_mutex_init (&m_mixer_lock);

  return TRUE;
}

void HmpAudioEncoderBin::check_update_rtp_timestamp (
                                   GstRTPBuffer     *rtpbuffer,
                                   mlineofferDetail *pmlineDetail,
                                   Client           *client,
                                   GstClockTime     clocktime)
{
  guint32 current_pkt_time = 0;
  offerDetail *pOfferDetails = &pmlineDetail->audioOfferDetails;
  gint32 rtp_time_diff = 0;
  GstClockTime  clocktime_diff_ms = 0;

  if (!pOfferDetails->uiTxTimestamp)
    pOfferDetails->uiTxTimestamp = 0;

  if(!pOfferDetails->lastClockTime)
     pOfferDetails->lastClockTime = clocktime;

  clocktime_diff_ms = clocktime - pOfferDetails->lastClockTime;
  clocktime_diff_ms = (clocktime_diff_ms / NANO_TO_MILLI_SEC);

  // check if the rtp time difference is too much . this will happen when
  // audio is stopped and started after a long time. In this case we need
  // to calculate the number of samples between the time gap and add it to
  // the rtp time stamp and send as RTP time stamp always shows running count
  // of the samples sent till now once the video is started. The count should
  // be incresing even though video is paused and started after a long time

  //if ((clocktime_diff_ms < 0) || (clocktime_diff_ms >= MILLI_TO_SEC))
  if ((TRUE == bSetMarkbit) && (clocktime_diff_ms >= MILLI_TO_SEC)) {
    rtp_time_diff = (clocktime_diff_ms / MILLI_TO_SEC ) * AUDIO_SAMPLES_1SEC
                      + ((clocktime_diff_ms % MILLI_TO_SEC )
                          * AUDIO_SAMPLES_1SEC) / MILLI_TO_SEC;
    //Decrement the uiTxTimeStamp by AUDIO_SAMPLE_TIME_OFFSET_20MS as we would have incremented
    //in the last run
    pOfferDetails->uiTxTimestamp -= AUDIO_SAMPLE_TIME_OFFSET_20MS;
    gst_rtp_buffer_set_marker (rtpbuffer, 1);
    bSetMarkbit = FALSE;
    XGLOG_INFO ("Final pkt_time for seqnum: %u is %u uiTxTimestamp: %u clocktime: %lld clocktime_diff_ms: %lld rtp_time_diff: %ld  ", gst_rtp_buffer_get_seq (rtpbuffer),
             current_pkt_time, pOfferDetails->uiTxTimestamp, clocktime, clocktime_diff_ms, rtp_time_diff );

  } else if (bDtxEnabled == TRUE && clocktime_diff_ms >= DTX_TIME_DIFF_MS){
     rtp_time_diff = (clocktime_diff_ms / MILLI_TO_SEC ) * AUDIO_SAMPLES_1SEC
                      + ((clocktime_diff_ms % MILLI_TO_SEC )
                          * AUDIO_SAMPLES_1SEC) / MILLI_TO_SEC;
    //Decrement the uiTxTimeStamp by AUDIO_SAMPLE_TIME_OFFSET_20MS as we would have incremented
    //in the last run
    pOfferDetails->uiTxTimestamp -= AUDIO_SAMPLE_TIME_OFFSET_20MS;
  }
  current_pkt_time = pOfferDetails->uiTxTimestamp + rtp_time_diff ;

  gst_rtp_buffer_set_timestamp (rtpbuffer, current_pkt_time);

  gst_rtp_buffer_set_seq (rtpbuffer, pOfferDetails->usSeqNum);
  // set marker bit if it's first packet transmitted
  if (!current_pkt_time && !pOfferDetails->usSeqNum)
    gst_rtp_buffer_set_marker (rtpbuffer, 1);

  pOfferDetails->lastClockTime = clocktime;
  pOfferDetails->uiTxTimestamp += AUDIO_SAMPLE_TIME_OFFSET_20MS + rtp_time_diff;
  pOfferDetails->usSeqNum++;
}

gboolean HmpAudioEncoderBin::check_dtx_enabled (string &sdp)
{
   gint dtx_pos = -1;
   if (sdp.empty()){
     return FALSE;
   }
   // if discontinuous transmission i,,e dtx is enabled, then usedtx=1 will be present in sdp
   dtx_pos = sdp.find ("usedtx=1");
   if (dtx_pos != string::npos)
     return TRUE;

   return FALSE;
}

GstPad *HmpAudioEncoderBin::Generate_new_Audiomixer_sinkpad()
{
  GstPad *audiomix_sinkpad = NULL;

  audiomix_sinkpad = gst_element_request_pad_simple (m_audiomixer, "sink_%u");
  XGLOG_INFO( "Generate_new_Audiomixer_sinkpad, creating new pad: %p ", audiomix_sinkpad);
  return audiomix_sinkpad;
}

/* HmpAudioEncoderBin derived class - end
*/

/* HmpVideoBin Base class - End
*/
#ifdef RECORD_ENABLED
/* HmpRecordingSendBin class - start
*/
HmpRecordingSendBin::HmpRecordingSendBin(string binName):
   m_bin(NULL), m_binName(binName), m_sendSocket(NULL), m_udpSrcPort(0),
   m_udpDstPort(0), m_sendIp(""), m_streamId(""), m_udpsink(NULL), m_udpSinkPad(NULL)
{
   XGLOG_INFO( "HmpRecordingSendBin construction start for RecordingSend Bin ");
   m_bin = GST_BIN(gst_bin_new(binName.c_str()));
}
HmpRecordingSendBin::~HmpRecordingSendBin()
{
  XGLOG_INFO( "HmpBin::~HmpRecordingSendBin destruction ended");

  if(!m_binName.empty())
    m_binName.clear();

  if(!m_sendIp.empty())
    m_sendIp.clear();

  if(m_udpSinkPad) {
    g_object_unref(GST_OBJECT(m_udpSinkPad));
    m_udpSinkPad = NULL;
  }

  if(m_udpDstPort > 0 && m_sendSocket){
    g_object_unref(m_sendSocket);
    m_sendSocket = NULL;
    m_udpDstPort = 0;
    m_udpSrcPort = 0;
  }
}

gboolean HmpRecordingSendBin::create_recording_send_bin(char *element_name)
{
  gchar sink_name[100] = {'\0'};
  sprintf (sink_name, "%s%s","multiUDPSink_",element_name);
  m_udpsink = gst_element_factory_make("multiudpsink", sink_name);
  gst_bin_add_many(m_bin, m_udpsink, NULL);
  return TRUE;
}

gboolean HmpRecordingSendBin::connect_send_socket()
{
  XGLOG_INFO( "connect_send_socket address");
  m_sendSocket = xGateHmpGstUtil::create_udpsocket(m_udpSrcPort);
  if(m_sendSocket){
    g_object_set(G_OBJECT(m_udpsink), "socket", m_sendSocket, NULL);
    g_object_set(G_OBJECT(m_udpsink),"async", FALSE,
                                     "sync", FALSE,
                                     "close-socket", FALSE,
                                     "send-duplicates", FALSE,
                                      NULL);
    string allClientAddrs = m_sendIp + ":" + to_string(m_udpDstPort);
    g_object_set(G_OBJECT(m_udpsink),"clients", allClientAddrs.c_str(), NULL);
    gst_element_sync_state_with_parent(m_udpsink);
    XGLOG_INFO( "connect_send_socket address %s is setted in udp socket ", allClientAddrs.c_str());
  }
  return TRUE;
}

gboolean HmpRecordingSendBin::send_start_event()
{
  XGLOG_INFO( "send start event for streamId:%s",m_streamId);
  GstEvent *pSegmentEvent = NULL;
  GstSegment *pSegment = NULL;
  if (m_streamId){
    GstEvent *pStreamEvent = NULL;
    m_udpSinkPad = gst_element_get_static_pad(m_udpsink,"sink");
    pStreamEvent = gst_event_new_stream_start (m_streamId);
    if (!pStreamEvent){
      XGLOG_WARN ("send_start_event could not be created for stream_id: %s ",m_streamId);
    }else {
      XGLOG_INFO ("send_start_event event: %p streamId: %s",
                                pStreamEvent, m_streamId);
      if (FALSE == gst_pad_send_event (m_udpSinkPad, pStreamEvent)){
        XGLOG_ERROR ("Failed to send_start_event for streamId: %s ", m_streamId);
      }
    }
  }
  /*Send segment event */
  pSegment = gst_segment_new();
  if (pSegment) {
    gst_segment_init( pSegment, GST_FORMAT_TIME);
    pSegment->rate  = 1.0;
    pSegment->start = 0;
    pSegment->stop = -1;

    pSegmentEvent = gst_event_new_segment (pSegment);
    if (pSegmentEvent){
      if (FALSE == gst_pad_send_event (m_udpSinkPad, pSegmentEvent)){
        XGLOG_ERROR ("Failed to send SEGMENT event for streamId: %s", m_streamId);
      }
    }
    gst_segment_free (pSegment);
  }
  return TRUE;
}

gboolean HmpRecordingSendBin::draw_dot_file()
{
  if(HAVE_GST_DEBUG_PRINTS && m_bin && !m_binName.empty()) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_bin, GST_DEBUG_GRAPH_SHOW_FULL_PARAMS, m_binName.c_str());
    return TRUE;
  }
  return FALSE;
}
/* HmpRecordingSendBin class - end
*/
#endif
/* HmpFrameBin derived class - start
*/
HmpFrameBin::HmpFrameBin(string binName, HmpVideoBinType binType, HmpCodecType codecType, \
    unsigned int dynamicCodec, string sdpstr, HmpCallDir callDir,
    xGateWebRtcBinType webRtcType, string webrtc_name) :
  HmpVideoBin(binName, binType, dynamicCodec), m_codecType(codecType),
  m_callDir(callDir), m_remoteSdpStr(sdpstr), m_localSdpStr("")
{
  m_dynamicCodec = dynamicCodec;
  m_webRtcBinType = webRtcType;
  m_webrtc_name = webrtc_name;
  m_ulNumAudioPktRcvd = 0;
  m_ulNumAudioPktSent = 0;
  m_ulNumVideoPktRcvd = 0;
  m_ulNumVideoPktSent = 0;
  rtp_video_rx_ssrc = 0;
  rtcp_sender_ssrc = 0;
  XGLOG_INFO( "HmpFrameBin construction start for webrtc_name: %s m_dynamicCodec: %d call_dir: %d",
		  m_webrtc_name.c_str(), m_dynamicCodec, m_callDir);

  if(m_callDir == EN_CALL_DIR_IN) {
    m_remoteSdpType = GST_WEBRTC_SDP_TYPE_OFFER;
  } else if(m_callDir == EN_CALL_DIR_OUT) {
    m_remoteSdpType = GST_WEBRTC_SDP_TYPE_ANSWER;
  } else {
    m_remoteSdpType = GST_WEBRTC_SDP_TYPE_ANSWER;
  }
  if (sdpstr.size()){
    sdpstr.clear();
  }
}

HmpFrameBin::~HmpFrameBin()
{
  XGLOG_INFO( "HmpFrameBin::~HmpFrameBin destruction start");
  if(!m_rtcp_stream_id.empty()){
    m_rtcp_stream_id.clear();
  }

  if(!m_rtcp_repair_stream_id.empty()){
    m_rtcp_repair_stream_id.clear();
  }

  if(!m_mid_video.empty()){
    m_mid_video.clear();
  }

  if(!m_rtcp_stream_id.empty()){
    m_rtcp_stream_id.clear();
  }

  if(!m_rtcp_repair_stream_id.empty()){
    m_rtcp_repair_stream_id.clear();
  }

  if(!m_webrtc_name.empty()){
    m_webrtc_name.clear();
  }

  if (!m_remoteSdpStr.empty()){
    m_remoteSdpStr.clear();
  }

  if (!m_localSdpStr.empty()){
    m_localSdpStr.clear();
  }

  if (!d_rtp_ep.empty()){
    d_rtp_ep.clear();
  }

  if (!o_rtp_ep.empty()){
    o_rtp_ep.clear();
  }

  if (!m_codec_payload_map.empty()){
    m_codec_payload_map.clear();
  }

  if (!AudioEncodingParam.empty()){
    AudioEncodingParam.clear();
  }

  if (!VideoEncodingParam_caps.empty()){
    VideoEncodingParam_caps.clear();
  }

  XGLOG_INFO( "HmpFrameBin::~HmpFrameBin destruction ended");
}

gboolean HmpFrameBin::construct(string webrtc_name)
{
  /*create webrtc element */
  if(!create_webrtcbin(webrtc_name)) {
    XGLOG_ERROR( "creating webrtcbin failed !!");
    return FALSE;
  }

  return TRUE;
}

gboolean HmpFrameBin::create_webrtcbin(string webrtc_name)
{
  StunConfig m_stunConfig;
  string stunConfig;
  GObject *iceAgent = NULL;
  gboolean  gbVal = TRUE;
  m_webrtcbin = gst_element_factory_make("webrtcbin", webrtc_name.c_str());
  if(!m_webrtcbin) {
    XGLOG_ERROR( "HmpFrameBin::create_webrtcbin failed !!");
    return FALSE;
  }

  this->o_rtp_ep.clear ();
  this->d_rtp_ep.clear ();
  this->m_negotiated_audio_payload_num = -1;
  this->m_negotiated_video_payload_num = -1;
  this->m_negotiated_audio_twcc_ext_Id = -1;
  this->m_negotiated_audio_level_Id = -1;
  this->m_negotiated_video_twcc_ext_Id = -1;
  this->m_negotiated_video_delay_playout_ext_Id = -1;
  this->m_ulNumAudioPktRcvd = 0;
  this->m_ulNumVideoPktRcvd = 0;
  this->bStartedRecvAudio = FALSE;

  this->audioSrcPad = NULL;
  this->audioSrcPadName = "";
  this->videoSrcPad = NULL;
  this->videoSrcPadName = "";

  gst_bin_add_many(m_bin, m_webrtcbin, NULL);

  g_object_set(G_OBJECT(m_webrtcbin),"ice-transport-policy",
            GST_WEBRTC_ICE_TRANSPORT_POLICY_ALL, NULL);

  m_bundlePolicy = GST_WEBRTC_BUNDLE_POLICY_MAX_BUNDLE;
  g_object_set(G_OBJECT(m_webrtcbin),"bundle-policy", m_bundlePolicy, NULL);

  g_object_get(G_OBJECT(m_webrtcbin), "ice-agent", &iceAgent, NULL);

  if(iceAgent) {
    g_object_set(G_OBJECT(iceAgent), "ice-udp", TRUE, NULL);
    g_object_set(G_OBJECT(iceAgent), "ice-tcp", FALSE, NULL);
    //g_object_set(G_OBJECT(iceAgent), "keepalive-conncheck", TRUE, NULL);
    g_object_unref (iceAgent);
  }
  // Jitter buffer value
  g_object_set(G_OBJECT(m_webrtcbin), "latency", (guint)JITTER_BUFFER_VAL, NULL);
  CONFIGSERVICE->readStunConfig(IP_TYPE_4);
  m_stunConfig.m_userName.erase();
  m_stunConfig.m_password.erase();
  m_stunConfig.m_stunDomain.erase();
  m_stunConfig.m_port = 0;

  m_stunConfig = CONFIGSERVICE->m_stunConfig;
  if (!m_stunConfig.m_userName.empty() &&
       !m_stunConfig.m_password.empty() &&
       !m_stunConfig.m_stunDomain.empty()){

    stunConfig = "turn://" + m_stunConfig.m_userName + ":" +
                    m_stunConfig.m_password + "@" + m_stunConfig.m_stunDomain +
                    ":" + to_string(m_stunConfig.m_port);
    CONFIGSERVICE->m_stunConfig.m_userName.erase();
    CONFIGSERVICE->m_stunConfig.m_password.erase();
    CONFIGSERVICE->m_stunConfig.m_stunDomain.erase();
    CONFIGSERVICE->m_stunConfig.m_port = 0;
    XGLOG_INFO( "HmpFrameBin::create_webrtcbin StunConfig: %s",stunConfig.c_str());
    g_signal_emit_by_name(m_webrtcbin, "add-turn-server", stunConfig.c_str(), &gbVal);
  }
  m_stunConfig.m_userName.erase();
  m_stunConfig.m_password.erase();
  m_stunConfig.m_stunDomain.erase();
  m_stunConfig.m_port = 0;

  CONFIGSERVICE->readStunConfig(IP_TYPE_6);
  m_stunConfig = CONFIGSERVICE->m_stunConfig;
  if (!m_stunConfig.m_userName.empty() && !m_stunConfig.m_password.empty()
     && !m_stunConfig.m_stunDomain.empty()){
      stunConfig = "turn://" + m_stunConfig.m_userName + ":" +
                       m_stunConfig.m_password + "@" + m_stunConfig.m_stunDomain +
                       ":" + to_string(m_stunConfig.m_port);

    XGLOG_INFO( "HmpFrameBin::create_webrtcbin StunConfig: %s",stunConfig.c_str());
    g_signal_emit_by_name(m_webrtcbin, "add-turn-server", stunConfig.c_str(), &gbVal);
  }
  XGLOG_INFO("m_webrtcbin %s created: %p jitter buffer: %d ms", webrtc_name.c_str(),
                               m_webrtcbin, JITTER_BUFFER_VAL);
  return TRUE;
}

gboolean HmpFrameBin::register_webrtcbin_callback(gpointer data)
{
  Client *client = data;
  XGLOG_INFO( "register_webrtcbin_callback called data: %p m_webrtcbin: %p ",
      data, m_webrtcbin);
  if(m_webrtcbin) {
    if(m_callDir == EN_CALL_DIR_OUT) {
      XGLOG_INFO("Registering on-negotiation needed ");
      g_signal_connect(G_OBJECT(m_webrtcbin), "on-negotiation-needed",
                     G_CALLBACK(&on_negotiation_needed_cb), data);
    }
    g_signal_connect(G_OBJECT(m_webrtcbin), "pad-added",
                     G_CALLBACK(&handle_pad_added_callback), data);

    g_signal_connect(G_OBJECT(m_webrtcbin), "pad-removed",
                     G_CALLBACK(&on_pad_removed_vid), data);

    g_signal_connect(G_OBJECT(m_webrtcbin), "notify::ice-gathering-state",
                     G_CALLBACK(&on_ice_gathering_state_change), data);

    g_signal_connect(G_OBJECT(m_webrtcbin), "notify::connection-state",
                      G_CALLBACK(&on_connection_state_change), data);

    g_signal_connect(G_OBJECT(m_webrtcbin), "notify::ice-connection-state",
                      G_CALLBACK(&on_ice_connection_state_change), data);
    return TRUE;
  }
  return FALSE;
}

GstWebRTCSessionDescription*
HmpFrameBin::create_webrtc_session_description(GstWebRTCSDPType sdpType,
                                               string &sdpStr)
{
  GstSDPMessage *sdp = NULL;
  GstWebRTCSessionDescription *webrtcSesDesp = NULL;

  if(sdpStr.empty()) {
    XGLOG_ERROR("create_webrtc_session_description failed. given sdp offer is empty !");
    return NULL;
  }

  int ret = gst_sdp_message_new (&sdp);
  if(GST_SDP_OK != ret) {
    XGLOG_ERROR( "create_webrtc_session_description failed to create new gst_sdp_message !");
    return NULL;
  }
  ret = gst_sdp_message_parse_buffer ((guint8 *)sdpStr.c_str(), sdpStr.length(), sdp);
  if(GST_SDP_OK != ret) {
    XGLOG_ERROR( "create_webrtc_session_description failed while parsing sdp string !");
    return NULL;
  }

  webrtcSesDesp = gst_webrtc_session_description_new(sdpType, sdp);
  if(!webrtcSesDesp) {
    XGLOG_ERROR( "Failed to create webrtcSesDesp !");
    return NULL;
  }

  return webrtcSesDesp;
}

gboolean HmpFrameBin::handle_sdp_offer(gpointer udata)
{
  GstWebRTCSessionDescription *offer = NULL;
  Client *client = (Client*)udata;
  gchar  *caps_string = NULL;
  guint  transceiver_direction = 0;
  HmpFrameBin *pFrameRecvBin  = client->m_FrameChannel->m_pHmpFrameBin;
  xGateWebRtcBinType  exGateWebRtcBinType = pFrameRecvBin->m_webRtcBinType;
  XGLOG_INFO("handling incoming sdp offer In handle sdp offer sdp_str:%s",m_remoteSdpStr.c_str());
  /*extract device type */
  client->eDeviceType = client->find_device_type (m_remoteSdpStr);

  offer = create_webrtc_session_description(GST_WEBRTC_SDP_TYPE_OFFER, m_remoteSdpStr);
  if(!offer) {
    XGLOG_ERROR( "handle_sdp_offer failed while creating struct from given sdp !");
    return FALSE;
  }

  /* Set remote description on our pipeline */
  GstPromise *promise = NULL;
  promise = gst_promise_new();
  if(!promise) {
    XGLOG_ERROR( "handle_sdp_offer failed while creating new gst_promise !");
    return FALSE;
  }

  /* configure the remote description */
  XGLOG_INFO("handle_sdp_offer remote sdp received client %p webrtc: %p", udata, m_webrtcbin);
  g_signal_emit_by_name(m_webrtcbin, "set-remote-description", offer, promise);

  /* We don't want to be notified when the action is done */
  gst_promise_interrupt(promise);
  gst_promise_unref(promise);

  GstWebRTCRTPTransceiver *trans = NULL;
  GstCaps *caps = NULL;
  m_isLocalSdpCreated = FALSE;

  /* Get the webrtcbin type and pass it to gst to create send,sendrecv only*/
  XGLOG_INFO( "Adding transceiver signal for Audio codec direction");
  transceiver_direction = GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_RECVONLY;
  caps = generate_MediaServer_caps(udata,MEDIA_CAPS_FOR_AUDIO, transceiver_direction);
  if (caps){
     g_signal_emit_by_name(m_webrtcbin, "add-transceiver",
                           transceiver_direction, caps, &trans);
#if 0
     if(trans)
     {
        XGLOG_INFO("Adding do-nack property to transiever");
        g_object_set(G_OBJECT(trans), "do-nack", (guint)TRUE, NULL);
        //g_object_set(G_OBJECT(trans), "fec-type", (guint)TRUE, NULL);
     }
#endif
     gst_caps_unref(caps);
     gst_object_unref(trans);
  }else{
     XGLOG_WARN("Audio caps is NULL Remote SDP may not have Video capability");
  }

  caps = generate_MediaServer_caps (udata,MEDIA_CAPS_FOR_VIDEO,
                                   transceiver_direction);
  if (caps){
    caps_string = gst_caps_to_string(caps);
    XGLOG_INFO( "Adding transceiver for video codecs %s", caps_string);
    g_signal_emit_by_name(m_webrtcbin, "add-transceiver",
                                transceiver_direction, caps, &trans);
    if(trans){
      XGLOG_INFO("Adding do-nack property to transiever");
      g_object_set(G_OBJECT(trans), "do-nack", (guint)TRUE, NULL);
      //g_object_set(G_OBJECT(trans), "fec-type", (guint) GST_WEBRTC_FEC_TYPE_ULP_RED, NULL);
    }
    gst_caps_unref(caps);
    gst_object_unref(trans);
    g_free (caps_string);
  }else{
    XGLOG_WARN("Video caps is NULL Remote SDP may not have Video capability");
  }
  promise = gst_promise_new_with_change_func((GstPromiseChangeFunc)on_answer_created_cb,
                                             (gpointer)client, NULL);
  g_signal_emit_by_name(m_webrtcbin, "create-answer", NULL, promise);

  gst_webrtc_session_description_free(offer);
  return TRUE;
}

xGateDeviceType Client::find_device_type (string  &str)
{
  xGateDeviceType eDevType = XGATE_DEVICE_TYPE_UNKNOWN;
  int pos = -1;
  int pos_end = -1;
  int new_pos = -1;
  string substr = "";
  /* one way to know remote end is mobile device or not is by checking the
   * presence of ADRMS string in sdp
   */
  if (str.size () <= 0)
    return eDevType;

  pos = str.find ("a=msid:");
  if (pos != string::npos){
    new_pos = str.find (":", pos);
    pos_end = str.find ("\n", new_pos);
    if (new_pos != string::npos &&  pos_end != string::npos){
      new_pos++;
      substr = str.substr (new_pos, (pos_end - new_pos));
    }
    if (substr.size () > 0){
      XGLOG_INFO ("Substring in pos: %d new_pos: %d pos_end: %d a=msid: line is %s" ,
                 pos, new_pos, pos_end, substr.c_str());
      if (strstr (substr.c_str(), "ARDAMS")){
        XGLOG_INFO ("Device Type is mobile");
        eDevType = XGATE_DEVICE_TYPE_MOBILE;
      }else {
        XGLOG_INFO ("Device Type is unknown");
      }
    }
  }
  return eDevType;
}

gboolean HmpFrameBin::handle_sdp_answer(gpointer udata)
{
  GstWebRTCSessionDescription *answer = NULL;
  Client *client = (Client *) udata;
  XGLOG_INFO( "handling incoming sdp answer");
  if(!m_remoteSdpStr.empty()){
     XGLOG_INFO( "Remote SDP (200 ok) received FROM client: %s SDP:  %s \n",
		     client->m_detail.m_callId.c_str(),
		     m_remoteSdpStr.c_str());
  }
  /*extract device type */
  client->eDeviceType = client->find_device_type (m_remoteSdpStr);

  answer = create_webrtc_session_description(GST_WEBRTC_SDP_TYPE_ANSWER, m_remoteSdpStr);
  if(!answer) {
    XGLOG_ERROR( "handle_sdp_answer failed while creating struct from given sdp !");
    return FALSE;
  }

  /* Set remote description on our pipeline */
  GstPromise *promise = NULL;
  promise = gst_promise_new ();
  if(!promise) {
    XGLOG_ERROR( "handle_sdp_answer failed while creating new gst_promise !");
    return FALSE;
  }

  /* configure the remote description */
  XGLOG_INFO("handle_sdp_answer configuring the remote sdp in webrtcbin %p", m_webrtcbin);
  g_signal_emit_by_name(m_webrtcbin, "set-remote-description", answer, promise);

  /* We don't want to be notified when the action is done */
  gst_promise_interrupt(promise);
  gst_promise_unref(promise);
  gst_webrtc_session_description_free(answer);

  return TRUE;
}

void HmpFrameBin::on_negotiation_needed_cb(GstElement *webrtcbin, gpointer udata)
{
  Client *client = (Client *)udata;
  HmpFrameChannel *frameRecvChannel = NULL;
  HmpFrameBin *frameRecvBin = NULL;
  map<void * , HmpFrameChannel* >::iterator it;
  string remote_sdp = "";

  frameRecvChannel = client->m_FrameChannel;
  frameRecvBin = frameRecvChannel->m_pHmpFrameBin;
  XGLOG_INFO("HmpFrameBin::on_negotiation_needed_cb triggered ! data: %p webrtc: %p ",
      udata,webrtcbin);
  frameRecvBin->m_isLocalSdpCreated = FALSE;
  configure_sdp_to_generate_offerOrAnswer (client,
                                 frameRecvChannel, frameRecvBin->m_remoteSdpStr,
                                 client->m_detail.m_callId,
                                 GENERATE_OFFER);
}

gboolean HmpFrameBin::configure_sdp_to_generate_offerOrAnswer (Client *client ,
                                                    void *pChannel,
                                                    string &SdpStr,
                                                    string o_rtp_callId,
                                                    int    offer_answer)
{
  HmpFrameChannel *pFrameChannel = (HmpFrameChannel *)pChannel;
  HmpFrameBin *frameRecvBin = pFrameChannel->m_pHmpFrameBin;
  GstElement *webrtcbin = frameRecvBin->m_webrtcbin;
  GstPromise *promise;
  gchar *g = NULL;
  GstCaps *caps = NULL;
  GstWebRTCRTPTransceiver *trans = NULL;
  GstStructure *caps_structure = NULL;
  GstWebRTCSessionDescription *offer = NULL;

    if (offer_answer == GENERATE_OFFER){
      promise = gst_promise_new_with_change_func(
                        (GstPromiseChangeFunc)on_offer_created_cb, client, NULL);
      g_signal_emit_by_name (webrtcbin, "create-offer", NULL, promise);
    }
    else if (offer_answer == GENERATE_ANSWER){
      offer = frameRecvBin->create_webrtc_session_description(GST_WEBRTC_SDP_TYPE_OFFER, SdpStr);
      if(!offer) {
        XGLOG_ERROR( "failed while creating WebRTCSessionDescription struct from given sdp !");
        return FALSE;
      }
      /* Set remote description on our pipeline */
      promise = gst_promise_new();
      if(!promise) {
        XGLOG_ERROR( "failed while creating new gst_promise !");
        return FALSE;
      }

      /* configure the remote description */
      g_signal_emit_by_name(frameRecvBin->m_webrtcbin, "set-remote-description", offer, promise);

      /* We don't want to be notified when the action is done */
      gst_promise_interrupt(promise);
      gst_promise_unref(promise);
      promise = gst_promise_new_with_change_func((GstPromiseChangeFunc)on_answer_created_cb, client, NULL);
      g_signal_emit_by_name (webrtcbin, "create-answer", NULL, promise);
      gst_webrtc_session_description_free(offer);
    }else if (offer_answer == CONFIGURE_REMOTE_DESCRIPTION){
      offer = frameRecvBin->create_webrtc_session_description(GST_WEBRTC_SDP_TYPE_OFFER, SdpStr);
      if(!offer) {
        XGLOG_ERROR( "failed while creating WebRTCSessionDescription struct from given sdp !");
        return FALSE;
      }
      /* Set remote description on our pipeline */
      promise = gst_promise_new();
      if(!promise) {
        XGLOG_ERROR( "failed while creating new gst_promise !");
        return FALSE;
      }

      /* configure the remote description */
      g_signal_emit_by_name(frameRecvBin->m_webrtcbin, "set-remote-description", offer, promise);

      /* We don't want to be notified when the action is done */
      gst_promise_interrupt(promise);
      gst_promise_unref(promise);
      gst_webrtc_session_description_free(offer);
    }else if (offer_answer == CONFIGURE_LOCAL_DESCRIPTION){
      GstWebRTCSessionDescription *answer = NULL;
      answer = frameRecvBin->create_webrtc_session_description(GST_WEBRTC_SDP_TYPE_ANSWER, SdpStr);
      if(!answer) {
        XGLOG_ERROR( "failed to create WebRTCSessionDescription struct from given sdp !");
        return FALSE;
      }
      /* Set local description on our pipeline */
      GstPromise *promise = NULL;
      promise = gst_promise_new ();
      if(!promise) {
        XGLOG_ERROR( "handle_sdp_answer failed while creating new gst_promise !");
        return FALSE;
      }

      /* configure the remote description */
      XGLOG_INFO("handle_sdp_answer configuring the remote sdp in webrtcbin %p",
                        frameRecvBin->m_webrtcbin);
      g_signal_emit_by_name(frameRecvBin->m_webrtcbin,
                            "set-local-description",
                             answer, promise);
      /* We don't want to be notified when the action is done */
      gst_promise_interrupt(promise);
      gst_webrtc_session_description_free(answer);
    }
    return TRUE;
}

gboolean HmpFrameBin::AddTransceivers (Client *client,
                                       ParticipantInfo sPartInfo,
                                       string remoteSdpStr,
                                       int offer_answer,
                                       guint transceiver_direction)
{
  HmpFrameBin *frameRecvBin = this;
  Client      *pMemberClient = NULL;
  GstElement *webrtcbin = frameRecvBin->m_webrtcbin;
  xGateSDPType sdpType = EN_SDP_TYPE_AUDIO_VIDEO;
  gchar *g = NULL;
  char *callId = NULL;
  xGateHmpConference *pxGateHmpConfObj = NULL;
  GstCaps *caps = NULL;
  mlineofferDetail *offerDetail = NULL;
  GstWebRTCRTPTransceiver *trans = NULL;
  GstStructure *caps_structure = NULL;
  GstWebRTCSessionDescription *offer = NULL;
  MLINE_OFFER_MAP::iterator it;
  if ((transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDONLY)
       || (sPartInfo.joiner_type != EN_JOINER_TYPE_SCREENSHARE)){
    SFUController *pSFUController = xGateSFUtil::getSFUController();
    pxGateHmpConfObj = pSFUController->find_conference_entry(client->m_detail.m_mgresourceId);
    if(!pxGateHmpConfObj){
       XGLOG_ERROR("Failed to find entry in conference");
       return FALSE;
    }
    /* if participant call_id is audio_only (represented for Audio mcu and) or
     * screen share (represented for Active speaker or screen share), create OfferDetail
     * with the original client call Id */

    if ((0 == strcmp (sPartInfo.call_id.c_str (), "audio_only"))
       || (0 == strcmp (sPartInfo.call_id.c_str (), "screenshare"))) {
      pMemberClient = client;
    } else {
      pMemberClient = pxGateHmpConfObj->get_participant_entry (sPartInfo.call_id);
      if (!pMemberClient){
        XGLOG_ERROR ("Failed to get client Pointer for callId: %s can't add transceiver",
                    sPartInfo.call_id.c_str());
        return FALSE;
      }
    }
    it = client->m_mlineOffer_map.find (sPartInfo.call_id);
    if (it != client->m_mlineOffer_map.end ()){
      XGLOG_INFO ("mlineofferDetails already exists for participant_list call_id: %s",
                        sPartInfo.call_id.c_str());
      return TRUE;
    }else{
       offerDetail = new (mlineofferDetail);
       if (!offerDetail){
          XGLOG_ERROR("Failed to allocate memory to store offer details");
          return FALSE;
       }
    }
  }
  sdpType = getSDPType (remoteSdpStr);
  if (EN_SDP_TYPE_AUDIO == (sdpType & EN_SDP_TYPE_AUDIO)
      && (sPartInfo.joiner_type != EN_JOINER_TYPE_SCREENSHARE )
      && (0 != strcmp (sPartInfo.call_id.c_str (), "screenshare"))) {
    gboolean bIsTransPresent = FALSE;
    bIsTransPresent = frameRecvBin->Check_Transceiver (client, remoteSdpStr, AUDIO_MCU_MID_LINE);
    if (bIsTransPresent){
      XGLOG_INFO ("audio_only participant present in SDP for mline %d checking if present in local SDP",
                   AUDIO_MCU_MID_LINE);
      bIsTransPresent = frameRecvBin->Check_Transceiver (client, m_localSdpStr, AUDIO_MCU_MID_LINE);
    }
    XGLOG_INFO ("result of call Check_Transceiver () bIsTransPresent %d", bIsTransPresent );
    if (!bIsTransPresent){
      caps = frameRecvBin->generate_MediaServer_caps(client,MEDIA_CAPS_FOR_AUDIO,
                          transceiver_direction);
      if (caps){
        g_signal_emit_by_name(webrtcbin, "add-transceiver",
                transceiver_direction, caps, &trans);
        if (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDONLY){
          caps_structure = gst_caps_get_structure (caps, 0);
          gst_structure_get (caps_structure,
               "ssrc", G_TYPE_UINT, &offerDetail->audioOfferDetails.uiTxSSRCNum,
               "payload", G_TYPE_INT, &offerDetail->audioOfferDetails.uiPayloadNum,
                NULL);
          XGLOG_INFO("Adding transceiver to have preferred Audio codecs ssrc: %u payload: %u",
                           offerDetail->audioOfferDetails.uiTxSSRCNum,
                           offerDetail->audioOfferDetails.uiPayloadNum);
          frameRecvBin->retrieved_audiossrc = offerDetail->audioOfferDetails.uiTxSSRCNum;
        }
        //gst_structure_free (caps_structure);
        gst_caps_unref(caps);
        gst_object_unref(trans);
      }
    }
    /* if participant is audio only and if offerDetails is already inserted return otherwise
     * goto insert_offer_Details */
    if (0 == strcmp (sPartInfo.call_id.c_str (), "audio_only")){
      if (bIsTransPresent){
         XGLOG_INFO ("Participant with audio_only already exists... returning");
         return TRUE;
       }else{
         goto insert_offer_details;
       }
    }
  }else {
    XGLOG_WARN("Audio caps is NULL .. Audio SDP may not be negotiated joiner_type: %d",
                   sPartInfo.joiner_type);
    offerDetail->audioOfferDetails.uiTxSSRCNum = 0;
    offerDetail->audioOfferDetails.sinkPadName = "";
  }
  if ((EN_SDP_TYPE_VIDEO == (sdpType & EN_SDP_TYPE_VIDEO))
          || (sPartInfo.joiner_type == EN_JOINER_TYPE_SCREENSHARE
             && client->m_detail.m_joinerType == EN_JOINER_TYPE_SCREENSHARE)
          && (0 != strcmp (sPartInfo.call_id.c_str (), "audio_only"))
          || (0 == strcmp (sPartInfo.call_id.c_str (), "screenshare"))){

    gboolean bIsTransPresent = FALSE;
    if (0 == strcmp (sPartInfo.call_id.c_str (), "screenshare")){
      bIsTransPresent = frameRecvBin->Check_Transceiver (client, remoteSdpStr, SCREENSHARE_MID_LINE);
      if (bIsTransPresent){
        XGLOG_INFO ("screen share participant present in SDP for mline %d checking if present in local SDP",
                    SCREENSHARE_MID_LINE);
        bIsTransPresent = frameRecvBin->Check_Transceiver (client, m_localSdpStr, SCREENSHARE_MID_LINE);
      }
    }
    XGLOG_INFO ("result of call Check_Transceiver () bIsTransPresent %d", bIsTransPresent );
    if (!bIsTransPresent){
      caps = frameRecvBin->generate_MediaServer_caps(client,MEDIA_CAPS_FOR_VIDEO,
                                 transceiver_direction);
      if (caps){
        g_signal_emit_by_name(webrtcbin, "add-transceiver",
               transceiver_direction, caps, &trans);
        if (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDONLY){
          caps_structure = gst_caps_get_structure (caps, 0);
          gst_structure_get (caps_structure,
               "ssrc", G_TYPE_UINT, &offerDetail->videoOfferDetails.uiTxSSRCNum,
               "payload", G_TYPE_INT,&offerDetail->videoOfferDetails.uiPayloadNum,
                NULL);
          XGLOG_INFO("Adding transceiver to have preferred video codecs ssrc: %u payload: %u",
                           offerDetail->videoOfferDetails.uiTxSSRCNum,
                           offerDetail->videoOfferDetails.uiPayloadNum);
        }
        //gst_structure_free (caps_structure);
        gst_caps_unref(caps);
      }
    }
    if(trans){
      XGLOG_INFO("Configuring the do-nack property to TRUE to transiever");
      g_object_set(G_OBJECT(trans), "do-nack", (guint)TRUE, NULL);
    /*g_object_set(G_OBJECT(trans), "fec-type", (guint) GST_WEBRTC_FEC_TYPE_ULP_RED,
           "percentage", (guint) 25,
           NULL); */
      gst_object_unref(trans);
    }
  }else{
     XGLOG_WARN("Video caps is NULL .. Video SDP may not be negotiated ");
  }
insert_offer_details:
  if (NULL != offerDetail){
    offerDetail->audioOfferDetails.bSendPkt = FALSE;
    offerDetail->videoOfferDetails.bSendPkt = FALSE;
    offerDetail->bPopulated = FALSE;
    offerDetail->eJoinerType = sPartInfo.joiner_type;
    offerDetail->audioOfferDetails.ulNumPktSent = 0;
    offerDetail->audioOfferDetails.usSeqNum = 0;
    offerDetail->audioOfferDetails.uiTxTimestamp = 0;
    offerDetail->audioOfferDetails.usPrevRcvdSeqNum = 0;
    offerDetail->audioOfferDetails.m_appsrc = NULL;
    offerDetail->audioOfferDetails.sinkPad = NULL;
    offerDetail->audioOfferDetails.sinkPadName = "";
    offerDetail->audioOfferDetails.pPeerElement = NULL;
    offerDetail->audioOfferDetails.pOldBufferList = NULL;
    offerDetail->audioOfferDetails.pNewBufferList = NULL;
    offerDetail->audioOfferDetails.lastClockTime = 0;
    memset (&offerDetail->audioOfferDetails.encoding_name, '\0',
                sizeof(offerDetail->audioOfferDetails.encoding_name));

    offerDetail->videoOfferDetails.ulNumPktSent = 0;
    offerDetail->videoOfferDetails.usSeqNum = 0;
    offerDetail->videoOfferDetails.uiTxTimestamp = 0;
    offerDetail->videoOfferDetails.usPrevRcvdSeqNum = 0;
    offerDetail->videoOfferDetails.uiPrevRTPTimestamp = 0;
    offerDetail->videoOfferDetails.sinkPadName = "";
    offerDetail->videoOfferDetails.sinkPad = NULL;
    offerDetail->videoOfferDetails.pPeerElement = NULL;
    offerDetail->videoOfferDetails.pOldBufferList = NULL;
    offerDetail->videoOfferDetails.pNewBufferList = NULL;
    memset (&offerDetail->videoOfferDetails.sSvcInfo, 0, sizeof(SVCInfo));
    memset (&offerDetail->videoOfferDetails.encoding_name, '\0',
                sizeof(offerDetail->videoOfferDetails.encoding_name));
    memset (&offerDetail->videoOfferDetails.sSvcInfo, 0,
                sizeof(offerDetail->videoOfferDetails.sSvcInfo));
    offerDetail->videoOfferDetails.sSvcInfo.sScalabilitStructInfo.ucNumSpatialLayers = 1;
    XGLOG_INFO ("created mlineofferDetail: %p Inserting offer_callId_str (%s) to map list in client: %s",
                    offerDetail,sPartInfo.call_id.c_str(),
                     client->m_detail.m_callId.c_str());

    client->m_mlineOffer_map.insert ({sPartInfo.call_id, offerDetail});
    offerDetail->callId_offer_generated = sPartInfo.call_id;
    offerDetail->pClient = client;
    client->m_mlineOffer_vector.push_back (offerDetail);
    it = client->m_mlineOffer_map.find (sPartInfo.call_id);
    if (it != client->m_mlineOffer_map.end ()){
       mlineofferDetail *pofferDetail = (mlineofferDetail*)it->second;
       XGLOG_INFO ("Found itr->first: %p offerDetail: %p entry in offerMap list map size: %d",
                         it->first,pofferDetail,
                         client->m_mlineOffer_map.size());
     }else{
       XGLOG_ERROR ("===== could not find entry in offerMap list map size: %d",
                                                     client->m_mlineOffer_map.size());
     }
   }
  return TRUE;
}

xGateSDPType HmpFrameBin::getSDPType (string sdp)
{
  int pos = -1;
  xGateSDPType eSdpType = EN_SDP_TYPE_UNKNOWN;
  if (!sdp.empty()){
    //XGLOG_INFO("getSDPType () sdp passed is %s \n",sdp.c_str());
    pos = sdp.find ("m=video", 0);
    if(pos != string::npos){
      eSdpType |= EN_SDP_TYPE_VIDEO;
    }
    pos = sdp.find ("m=audio", 0);
    if(pos != string::npos){
      eSdpType |= EN_SDP_TYPE_AUDIO;
    }
  }
  return eSdpType;
}

void HmpFrameBin::on_offer_created_cb(GstPromise * promise, gpointer udata)
{
  GstElement *webrtcbin = NULL;
  GstWebRTCSessionDescription *offer = NULL;
  const GstStructure *reply;
  GstSDPConnection *psGstSDPconnectInfo  = NULL;
  GstSDPMedia *psGstSDPMedia = NULL;
  stringstream GstSDP;
  guint ice_gather_state = 0;
  Client  *client = (Client *) udata;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  int ret;
  HmpFrameChannel *pHmpFrameChanel = (HmpFrameChannel *) client->m_FrameChannel;
  HmpFrameBin *frameRecvBin = pHmpFrameChanel->m_pHmpFrameBin;
  webrtcbin = frameRecvBin->m_webrtcbin;
  XGLOG_INFO( "HmpFrameBin::on_offer_created_cb triggered webrtc: %p TO client: %s",
		  webrtcbin, client->m_detail.m_callId.c_str());
  ret = gst_promise_wait(promise);
  if(ret != GST_PROMISE_RESULT_REPLIED) {
    XGLOG_ERROR( "HmpFrameRecvBin::on_offer_created_cb failed. gst_promise_wait failed !");
    return;
  }

  reply = gst_promise_get_reply(promise);
  gst_structure_get(reply, "offer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &offer, NULL);
  gst_promise_unref(promise);
  if(offer){
    frameRecvBin->extract_extension_id(offer);
    gchar* desc = gst_sdp_message_as_text(offer->sdp);
    XGLOG_INFO( "offer created by Gstreamer for client: %s :\n%s\n",
                             client->m_detail.m_callId.c_str(), desc);
    promise = gst_promise_new();
    g_signal_emit_by_name(webrtcbin, "set-local-description", offer, promise);
    g_object_get(G_OBJECT(webrtcbin),"ice-gathering-state",&ice_gather_state,NULL);
    /* If ICE gathering state is completed, no more ice candidates will be generated.
     * if Previously local SDP is generated,then this one is SDP update,
     * Send updated SDP here itself
     */
    if (GST_WEBRTC_ICE_GATHERING_STATE_COMPLETE == ice_gather_state){
      frameRecvBin->m_localSdpStr.clear();
      frameRecvBin->m_localSdpStr = gst_sdp_message_as_text(offer->sdp);

      xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg ();
      ACE_Time_Value tval ((time(NULL) ));
      ACE_Message_Block* pAmb = NULL;
      tval += ACE_Time_Value(0,1);
      pMsg->m_msgType = XGATE_CLIENT_SEND_UPDATED_SDP;
      pMsg->param1 = static_cast <void*> (pHmpFrameChanel);
      pMsg->param2 = static_cast <void*> (client);
      pMsg->pClient = client;
      if (pSFUController->m_ThreadPool_info.m_event_pool) {
        GError *error = NULL;
        g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, &error);
        if (error != NULL){
          XGLOG_ERROR ("Failed to push event to thread pool");
          g_error_free (error);
        }
      }
      else{
        XGLOG_ERROR ("event pool thread is null... can't push event");
      }
    }
    gst_promise_interrupt(promise);
    gst_promise_unref(promise);
    gst_webrtc_session_description_free(offer);
    g_free(desc);
  }else{
    XGLOG_ERROR("Offer created by webrtcbin is NULL");
  }
}

static void HmpFrameBin::RegisterRTCPCallback(void *data)
{
  Client *client = (Client *)data;
  HmpFrameBin *frameRecvBin = client->m_FrameChannel->m_pHmpFrameBin;
  GstElement *webrtcbin = frameRecvBin->m_webrtcbin;
  GstElement *rtpbin = NULL;
  rtpbin = gst_bin_get_by_name (GST_BIN(webrtcbin), "rtpbin");
  XGLOG_INFO( "Entered RegisterRTCPCallback () rtpbin pointer present inside webrtcbin is %p ", rtpbin);
  if(rtpbin)
  {
    /* Getting the internal session pointer callback for media index based on
     * the bundle policy. If bundle policy is bundle then there will be only
     * one session for audio/video with session id 0 else 2 sessions with
     * session id 0 for audio and session id 1 for video
     */
    frameRecvBin->rtp_session = NULL;
    frameRecvBin->gst_rtp_session = NULL;
    if (frameRecvBin->m_bundlePolicy == GST_WEBRTC_BUNDLE_POLICY_NONE){
      g_signal_emit_by_name (rtpbin, "get-internal-session", 1, &frameRecvBin->rtp_session);
      g_signal_emit_by_name (rtpbin, "get-session", 1, &frameRecvBin->gst_rtp_session);
    }
    else{
      g_signal_emit_by_name (rtpbin, "get-internal-session", 0, &frameRecvBin->rtp_session);
      g_signal_emit_by_name (rtpbin, "get-session", 0, &frameRecvBin->gst_rtp_session);
    }
    XGLOG_INFO( "registering signal to rtp session: %p get RTCP packet webrtcbin: %p",
    	         frameRecvBin->rtp_session, webrtcbin);
    if(frameRecvBin->rtp_session){
        g_signal_connect(G_OBJECT(frameRecvBin->rtp_session),"on-receiving-rtcp",
                         G_CALLBACK (on_rtcp_received),(gpointer) data);

      //g_signal_connect(G_OBJECT(frameRecvBin->rrtp_session),
      //"on-sending-rtcp",G_CALLBACK (on_rtcp_send),(gpointer) data);
    }
 #ifdef ENABLE_TWCC_TX_PATH
    if (frameRecvBin->gst_rtp_session){
      g_signal_connect(G_OBJECT(frameRecvBin->gst_rtp_session), "notify::twcc-stats",
                       G_CALLBACK (client->on_twcc_stats_recv), (gpointer) data);
    }
 #endif
    gst_object_unref (rtpbin);
  }
}

gboolean HmpFrameBin::Check_Transceiver (Client *client, string &remoteSdpStr, guint mlineIdx)
{
  gboolean bIsPresent = FALSE;
  size_t pos =  string::npos;
  string str = this->m_localSdpStr;
retry:
  if (str.size ()){
    pos = m_localSdpStr.find ("a=group:BUNDLE");
    if(pos != string::npos){
      pos = m_localSdpStr.find (mlineIdx, pos);
    }
    if (pos != string::npos){
      XGLOG_INFO ("mlineIdx: %d found at pos: %d ", mlineIdx, pos);
      bIsPresent = TRUE;
    }
  }else {
    str = remoteSdpStr;
    goto retry;
  }
  if (!bIsPresent) {
    MLINE_OFFER_MAP::iterator it;
    string str = "";
    if (mlineIdx == AUDIO_MCU_MID_LINE)
      str = "audio_only";
    else if (mlineIdx == SCREENSHARE_MID_LINE)
      str = "screenshare";

    it = client->m_mlineOffer_map.find (str);
    if (it != client->m_mlineOffer_map.end ()){
       mlineofferDetail *pofferDetail = (mlineofferDetail*)it->second;
       XGLOG_INFO ("Found itr->first: %p offerDetail: %p entry in offerMap list map size: %d",
                         it->first,pofferDetail,
                         client->m_mlineOffer_map.size());
       bIsPresent = TRUE;
     }else{
       XGLOG_ERROR ("===== could not find entry in offerMap list map size: %d",
                                                     client->m_mlineOffer_map.size());
       bIsPresent = FALSE;
     }
  }
  return bIsPresent;
}

int HmpFrameBin::get_extension_id(gchar* pval)
{
 int retval = -1;
 string s(pval);
 size_t pos = s.find(' ');
 if(pos != string::npos){
   char extId[pos + 2];
   for(int i = 0; i < pos + 1; i++){
     extId[i] = pval[i];
   }
   extId[pos + 1] = '\0';
   retval = atoi(extId);
   XGLOG_INFO("Found extId  is %d",retval);
  }
  else
   {
     XGLOG_ERROR("Invalid extmap found with no space delimiter");
  }
  return retval;
}

void HmpFrameBin::extract_extension_id(GstWebRTCSessionDescription *pSDP)
{
  GstSDPMessage *pmsg = pSDP->sdp;
  guint len = gst_sdp_message_medias_len(pmsg);
  XGLOG_INFO("Number of media's found is %d ",len);
  const GstSDPMedia *psdpMedia = NULL;
  for(int i = 0; i < len; i++){
    psdpMedia =  gst_sdp_message_get_media(pmsg,i);
    if(!strcmp(gst_sdp_media_get_media(psdpMedia),"video")){
      break;
    }
  }
  len = gst_sdp_media_attributes_len(psdpMedia);
  XGLOG_INFO("Number of attributes found in SDP Video is %d",len);
  for(int i = 0; i < len; i++)
  {
    const GstSDPAttribute* pattrib = gst_sdp_media_get_attribute (psdpMedia,i);
    char *p = NULL;
    if(!strcmp(pattrib->key,"extmap"))
    {
      if(p = strstr(pattrib->value,"repaired-rtp-stream-id")){
	int extId = get_extension_id(pattrib->value);
        m_repair_rtp_stream_ext_id = extId;
	XGLOG_INFO("Found extId for repaired-rtp-stream-id is %d",extId);
      }
      else
      {
        if(p = strstr(pattrib->value,"rtp-stream-id")){
	  int extId = get_extension_id(pattrib->value);
	  m_rtp_stream_ext_id = extId;
	  XGLOG_INFO("Found extId for rtp-stream-id is %d",extId);
        }
      }
    }
    else
    {
      if(!strcmp(pattrib->key,"mid")){
        m_mid_video = pattrib->value;
        XGLOG_INFO("Found val for this attrib as %s and length as %d and stored as %s",
                    pattrib->value,strlen(pattrib->value),m_mid_video.c_str());
      }
    }
  }
}

void HmpFrameBin::on_answer_created_cb(GstPromise * promise, gpointer udata)
{
  HmpFrameBin *frameRecvBin = NULL;
  GstElement *webrtcbin = NULL;
  GstElement *rtpbin= NULL;
  GstWebRTCSessionDescription *answer = NULL;
  const GstStructure *reply;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  Client *client = (Client *)udata;
  guint ice_gather_state = 0;
  int ret;

  HmpFrameChannel *pHmpFrameChanel = client->m_FrameChannel;
  if(!pHmpFrameChanel){
    XGLOG_ERROR("Failed to find HmpFrameChannel for promise: %p",promise);
    return;
  }
  frameRecvBin = pHmpFrameChanel->m_pHmpFrameBin;
  webrtcbin = frameRecvBin->m_webrtcbin;

  ret = gst_promise_wait(promise);
  if(ret != GST_PROMISE_RESULT_REPLIED) {
    XGLOG_ERROR( "HmpFrameBin::gst_promise_wait failed !");
    return;
  }

  reply = gst_promise_get_reply (promise);
  gst_structure_get(reply, "answer",
      GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &answer, NULL);
  gst_promise_unref (promise);
  if(answer){
    frameRecvBin->extract_extension_id(answer);
    XGLOG_INFO ("answer generated by Gstreamer for callId: %s",
                  client->m_detail.m_callId.c_str());

    promise = gst_promise_new ();
    g_signal_emit_by_name (webrtcbin, "set-local-description", answer, promise);
    gst_promise_interrupt (promise);
    gst_promise_unref (promise);
    gchar *text = gst_sdp_message_as_text(answer->sdp);
    if (frameRecvBin->m_localSdpStr.size()){
       frameRecvBin->m_localSdpStr.clear();
    }
    frameRecvBin->m_localSdpStr = text;
    g_object_get(G_OBJECT(webrtcbin),"ice-gathering-state",&ice_gather_state,NULL);
    /* If ICE gathering state is completed, no more ice candidates will be generated.
     * if Previously local SDP is generated,then this one is SDP update,
     * Send updated SDP here itself
     */
    if (GST_WEBRTC_ICE_GATHERING_STATE_COMPLETE == ice_gather_state){
      xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg ();
      ACE_Time_Value tval ((time(NULL) ));
      ACE_Message_Block* pAmb = NULL;
      tval += ACE_Time_Value(0,1);
      pMsg->m_msgType = XGATE_CLIENT_SEND_UPDATED_SDP;
      pMsg->param1 = static_cast <void*> (pHmpFrameChanel);
      pMsg->param2 = static_cast <void*> (client);
      pMsg->pClient = client;
      if (pSFUController->m_ThreadPool_info.m_event_pool) {
        GError *error = NULL;
        g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, &error);
        if (error != NULL){
          XGLOG_ERROR ("Failed to push event to thread pool");
          g_error_free (error);
        }
      }
      else{
        XGLOG_ERROR ("event pool thread is null... can't push event");
      }
    }
    g_free(text);
    gst_webrtc_session_description_free(answer);
  }else{
    XGLOG_ERROR("Answer from webrtcbin is NULL");
  }
}

void HmpFrameBin::handle_pad_added_callback (GstElement *webrtcbin, GstPad *pad, gpointer data)
{
  gchar *padName = NULL;
  gchar* elementName = NULL;
  GstPadDirection padDir = GST_PAD_UNKNOWN;
  HmpFrameChannel *frameRecvChannel = NULL;
  HmpFrameBin *frameRecvBin = NULL;
  padName = gst_pad_get_name(pad);
  elementName = gst_element_get_name(webrtcbin);
  Client *client = (Client *)data;
  frameRecvChannel = client->m_FrameChannel;
  frameRecvBin = frameRecvChannel->m_pHmpFrameBin;

  XGLOG_INFO("handle_pad_added_callback client: %p call_id: %s webrtcbin: %p element_name: %s pad_name: %s ",
		  client, client->m_detail.m_callId.c_str(), webrtcbin, elementName, padName);
  padDir = gst_pad_get_direction(pad);
  switch(padDir) {
    case GST_PAD_SINK:
      {
        XGLOG_INFO("Received sink pad: %s .. element: %s", padName, elementName);
        if(!frameRecvBin->handle_sinkpad_event (pad, client)) {
          XGLOG_ERROR( "handle_sinkpad_event failed for element: %s for pad %s !", elementName, padName);
        } else {
          XGLOG_INFO("handle_pad_added successfull client: %p m_webrtc: %p pad: %s for element: %s ",
                 client, frameRecvBin->m_webrtcbin, padName, elementName);
        }
	frameRecvChannel->draw_dot_file();
        break;
      }
    case GST_PAD_SRC:
      {
        XGLOG_INFO("Received src pad event.. event Calling handle_srcpad_event");
        if(!frameRecvBin->handle_srcpad_event (pad, client)) {
          XGLOG_ERROR( "failed for element: %s while handling pad: %s !",
                               elementName, padName);
        } else {
          XGLOG_INFO( "handle_srcpad_event successfull pad: %s for element: %s ",
                               padName, elementName);
          frameRecvBin->bStartedRecvAudio = TRUE;
        }
	frameRecvChannel->draw_dot_file();
        break;
      }
    case GST_PAD_UNKNOWN:
      XGLOG_INFO( "HmpFrameBin::on_pad_added unknown padDir %d", padDir);
      break;
  }
  if (padName){
    g_free(padName);
  }
  if (elementName){
    g_free(elementName);
  }
  XGLOG_INFO("Exiting the on_pad_added () function");
}

gboolean HmpFrameBin::get_rtp_stream_info(GstRTCPPacket *rtcp_packet,Client *client)
{
  guint num_items = gst_rtcp_packet_sdes_get_item_count(rtcp_packet);
  GstRTCPSDESType type;
  guint8 len;
  guint8  *data = NULL;
  guint32 ssrc;
  int count = 0;
  map<guint32,string>::iterator it;
  //XGLOG_INFO("Number of items in the packet are %d",num_items);
  for(int i = 0; i < num_items; i++)
  {
    gst_rtcp_packet_sdes_copy_entry(rtcp_packet,&type,&len,&data);
    //ssrc = gst_rtcp_packet_sdes_get_ssrc(rtcp_packet);
    //XGLOG_INFO("type : %d for rtp-stream-id:%s,ssrc:%u,count:%d",type,data,ssrc,count);
    if(type == RTP_STREAM_ID || type == REPAIR_RTP_STREAM_ID){
      ssrc = gst_rtcp_packet_sdes_get_ssrc(rtcp_packet);
      if(type == RTP_STREAM_ID){
        if((it = m_rtcp_stream_id.find(ssrc)) == m_rtcp_stream_id.end()){
            m_rtcp_stream_id.insert({ssrc,string((char*)data,len + 1)});
        }
      }
      else if(type == REPAIR_RTP_STREAM_ID){
        if((it = m_rtcp_repair_stream_id.find(ssrc)) == m_rtcp_repair_stream_id.end()){
            m_rtcp_repair_stream_id.insert({ssrc,string((char*)data,len + 1)});
        }
      }

       XGLOG_INFO("SSRC : %u for rtp-stream-id:%s and type:%d",ssrc,data,type);
    }
      g_free(data);
     gst_rtcp_packet_sdes_next_item(rtcp_packet);
  }
  return TRUE;
}

static void HmpFrameBin::DecodeRTCPPacket(GstRTCPPacket *rtcp_packet,
		                          HmpFrameBin *pFrameBin,
					  Client *client)
{
  GstRTCPType  RTCPPktType = GST_RTCP_TYPE_INVALID;
  guint16      u16PktLength = 0;
  unsigned char *p = NULL;
  if(NULL == rtcp_packet)
  {
    XGLOG_ERROR("RTCP packet pointer is NULL");
  }
  else{
    /* get the packet type and packet length*/
    RTCPPktType = gst_rtcp_packet_get_type(rtcp_packet);
    u16PktLength = gst_rtcp_packet_get_length(rtcp_packet);
    /* As per RTCP protocol, length field is 32-bit words minus one.convert the PktLength in bytes */
    u16PktLength = (u16PktLength + 1) * 4;
#ifdef DUMP_RTCP_PKT
    XGLOG_INFO("RTCP packet type: %d Length of Pkt: %d",RTCPPktType,u16PktLength);
    XGLOG_INFO("rtcp_pkt: %p offset: %d ",rtcp_packet->rtcp,rtcp_packet->offset);
    XGLOG_INFO("RTCP MEM MAP info data: %p total RTCP buffer size: %d ",
                    rtcp_packet->rtcp->map.data,rtcp_packet->rtcp->map.size);
    p = (unsigned char*)&rtcp_packet->rtcp->map.data[rtcp_packet->offset];
    for (guint16 i =0 ; i < u16PktLength; i+=4)
    {
      XGLOG_INFO("%2x %2x %2x %2x ",
      p[i],p[i+1],p[i+2],p[i+3]);
    }
#endif
    switch(RTCPPktType)
    {
      case GST_RTCP_TYPE_SR:
           //XGLOG_INFO("Sender Report pkt");
       break;
      case GST_RTCP_TYPE_RR:
           //XGLOG_INFO("Receiver Report pkt");
       break;
      case GST_RTCP_TYPE_SDES:
           //XGLOG_INFO("Received Source Description (SDES) Pkt ");
           get_rtp_stream_info(rtcp_packet,client);
       break;
      case GST_RTCP_TYPE_BYE:
           XGLOG_INFO("RTCP BYE Pkt received from webrtc: %s o_rtp_ep: %s d_rtp_ep: %s client: %s",
      	              pFrameBin->m_webrtc_name.c_str(),
      	              pFrameBin->o_rtp_ep.c_str(),
      	              pFrameBin->d_rtp_ep.c_str(),
      	              client->m_detail.m_callId.c_str());
       break;
      case GST_RTCP_TYPE_APP:
           XGLOG_INFO("Application Defined");
       break;
      case GST_RTCP_TYPE_RTPFB:
           //XGLOG_INFO("Transport feedback GST_RTCP_TYPE_RTPFB");
           //DecodeRTCPTransportFeedBack(rtcp_packet,pFrameBin,client);
       break;
      case GST_RTCP_TYPE_PSFB:
           //XGLOG_INFO("Payload specific feedback GST_RTCP_TYPE_PSFB");
           DecodeRTCPPSFBPacket(rtcp_packet,pFrameBin,client);
       break;
      case GST_RTCP_TYPE_XR:
           //XGLOG_INFO("Extended report");
       break;
      default:
       XGLOG_INFO("Invalid RTCP pkt received");
       RTCPPktType = GST_RTCP_TYPE_INVALID;
    }
  }
}

void HmpFrameBin::DecodeRTCPPSFBPacket(GstRTCPPacket *rtcp_packet,
		                       HmpFrameBin *pFrameBin,
				       Client *src_client)
{
  GstRTCPFBType RTCPFBType = GST_RTCP_FB_TYPE_INVALID;
  guint16      u16PktLength = 0;
  unsigned char *p = NULL;
  Client        *dst_client = NULL;
  if(NULL == rtcp_packet)
  {
    XGLOG_ERROR("RTCP Payload specific Feedback packet pointer is NULL");
  }
  else{
    /* get the packet type and packet length*/
    RTCPFBType = gst_rtcp_packet_fb_get_type(rtcp_packet);
    /*  length of the FCI packet in 32-bit words. convert to bytes*/
    u16PktLength = gst_rtcp_packet_fb_get_fci_length(rtcp_packet);
    u16PktLength *= 4;
    //XGLOG_INFO("Payload specific feedback Type: %d Pkt Length: %d",
    //		RTCPFBType, u16PktLength);
    switch(RTCPFBType)
    {
      /* NACK and PLI enum values are same for Feedback. Handle one of them */
      case GST_RTCP_PSFB_TYPE_PLI:
      case GST_RTCP_PSFB_TYPE_FIR:
      {
        SFUController *pSFUController = xGateSFUtil::getSFUController();

        if(!src_client){
          XGLOG_ERROR("src_client corresponding to for webrtc: %p is NULL",
                       pFrameBin->m_webrtcbin);
          break;
        }
        if(EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO == src_client->m_detail.m_callType){
          guint media_ssrc = 0;
          guint sender_ssrc = 0;
          string callId ;
          guint index = 0;
          mlineofferDetail *mlineDetails = NULL;
          SFUController *pSFUController = xGateSFUtil::getSFUController();
          MgMediaDetail sMediaDetail;
          /* 1. get the media ssrc of packet received.
           * 2. From the vector , get the offer detail and compare it with media ssrc.
                get the callId corresponding to that ssrc
           * 3. Send the key frame request to the call id extracted
           */
          media_ssrc = gst_rtcp_packet_fb_get_media_ssrc (rtcp_packet);
          if (media_ssrc <= 0){
            XGLOG_INFO ("Failed to extract media ssrc");
            break;
          }
          XGLOG_INFO("Received GST_RTCP_PSFB_TYPE_PLI from Client: %s for media_ssrc: %u",
                src_client->m_detail.m_callId.c_str(), media_ssrc);
          for (index = 0; index < src_client->m_mlineOffer_vector.size (); index++){
            mlineDetails = src_client->m_mlineOffer_vector.at(index);
            if (mlineDetails->videoOfferDetails.uiTxSSRCNum == media_ssrc){
              sMediaDetail.call_id =  mlineDetails->callId_offer_generated;
              break;
            }
          }
          if (!sMediaDetail.call_id.empty()){
            if (strstr (sMediaDetail.call_id.c_str(), "screenshare")
                && (!src_client->m_mid3CallId.empty())){
               sMediaDetail.call_id.erase ();
               sMediaDetail.call_id = src_client->m_mid3CallId;
             }
            sMediaDetail.mgresource_id = src_client->m_detail.m_mgresourceId;
            sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
            sMediaDetail.joiner_type = src_client->m_detail.m_joinerType;
            pSFUController->send_msg_sfu_client(&sMediaDetail,src_client->m_detail,
    	                       EN_XGATE_MG_MEDIA_REQUEST_KEY_FRAME);
          }else {
            XGLOG_ERROR ("sMediaDetail.call_id is empty media_ssrc: %lu", media_ssrc);
          }
        }
      }
      break;
      case GST_RTCP_PSFB_TYPE_SLI:
      {
        XGLOG_INFO("Received GST_RTCP_PSFB_TYPE_SLI pkt");
      }
      break;
      case GST_RTCP_PSFB_TYPE_RPSI:
      {
        XGLOG_INFO("Received GST_RTCP_PSFB_TYPE_RPSI pkt");
      }
      break;
      default:
        //XGLOG_INFO("FB Pkt type: %d not handled",RTCPFBType);
      break;
    }
  }
}

static void HmpFrameBin::DecodeRTCPTransportFeedBack(GstRTCPPacket *rtcp_packet,
		                                     HmpFrameBin *pFrameBin,
						     Client *client)
{
  GstRTCPFBType RTCPFBType = GST_RTCP_FB_TYPE_INVALID;
  guint16      u16PktLength = 0;
  unsigned char *p = NULL;
  if(NULL == rtcp_packet)
  {
      XGLOG_ERROR("RTCP transport specific Feedback packet pointer is NULL");
  }
  else{
    /* get the packet type and packet length*/
    guint32 sender_ssrc = 0;
    guint32 media_ssrc = 0;
    RTCPFBType = gst_rtcp_packet_fb_get_type(rtcp_packet);
      sender_ssrc = gst_rtcp_packet_fb_get_sender_ssrc (rtcp_packet);
    media_ssrc = gst_rtcp_packet_fb_get_media_ssrc (rtcp_packet);

     /*  length of the FCI packet in 32-bit words. convert to bytes*/
     u16PktLength = gst_rtcp_packet_fb_get_fci_length(rtcp_packet);
     u16PktLength *= 4;
    //XGLOG_INFO("Transport feedback Type: %d Pkt Length: %d ",
                 //RTCPFBType, u16PktLength);
    if (RTCPFBType == GST_RTCP_RTPFB_TYPE_NACK){
      /* NACK and PLI enum values are same for Feedback. Handle one of them */
      unsigned short u16Seqnum = 0;
         //XGLOG_INFO("Received GST_RTCP_RTPFB_TYPE_NACK pkt");
      unsigned char *p = (unsigned char*)&rtcp_packet->rtcp->map.data[rtcp_packet->offset];
      u16Seqnum =  ((p[12] & 0xF0) >> 4) * 4096 + (p[12] & 0x0F)*256 +
                   ((p[13] & 0xF0) >> 4) * 16 + (p[13] & 0x0F);
    //    XGLOG_INFO("NACK rcvd from SSRC: 0x%08X about SSRC: 0x%08X lost pkt seqnum: %u blp: 0x%x%x",
    //		        sender_ssrc, media_ssrc,u16Seqnum,p[14],p[15]);
    }else if (RTCPFBType == GST_RTCP_RTPFB_TYPE_TMMBR){
      XGLOG_INFO("Received Temporary Maximum Media Stream Bit Rate Request pkt");
    }else if (RTCPFBType == GST_RTCP_RTPFB_TYPE_TMMBN){
      XGLOG_INFO("Received Temporary Maximum Media Stream Bit Rate Notification pkt");
    }else if (RTCPFBType == GST_RTCP_RTPFB_TYPE_RTCP_SR_REQ){
      XGLOG_INFO("Received Request an SR packet for early synchronization pkt");
    } else if (RTCPFBType == GST_RTCP_RTPFB_TYPE_TWCC){
      //XGLOG_INFO("Received TWCC pkt");
    }else{
      XGLOG_INFO("FB Pkt type: %d not handled",RTCPFBType);
    }
  }
}

void HmpFrameBin::on_rtcp_received(GstElement *rtpsession, GstBuffer *pRTCPBuffer,gpointer udata)
{
  GstRTCPBuffer rtcp_buffer;
  GstRTCPPacket rtcp_packet ;
  guint         num_rtcp_pkts = 0;
  guint         uiIndex = 0;
  Client        *src_client = (Client *)udata;
  HmpFrameBin *pFrameBin = src_client->m_FrameChannel->m_pHmpFrameBin;
  //XGLOG_INFO("Entered on_rtcp_received rtpsession: %p RTCPBuffer: %p webrtcbin: %p",
  //			rtpsession, pRTCPBuffer, pFrameBin->m_webrtcbin);
  rtcp_buffer.buffer = NULL;
  gst_rtcp_buffer_map (pRTCPBuffer, GST_MAP_READ, &rtcp_buffer);
  num_rtcp_pkts = gst_rtcp_buffer_get_packet_count (&rtcp_buffer);
//  XGLOG_INFO("Num of RTCP pkts: %d",num_rtcp_pkts);
  for (uiIndex  = 0; uiIndex < num_rtcp_pkts; uiIndex++)
  {
    if(!uiIndex){
 //   XGLOG_INFO("extracting first RTCP packet");
      gst_rtcp_buffer_get_first_packet(&rtcp_buffer,&rtcp_packet);
    }
    else{
 //   XGLOG_INFO("extracting next RTCP packet");
      gst_rtcp_packet_move_to_next(&rtcp_packet);
    }
    pFrameBin->DecodeRTCPPacket(&rtcp_packet, pFrameBin, src_client);
  }
  gst_rtcp_buffer_unmap (&rtcp_buffer);
}

static gboolean HmpFrameBin::on_rtcp_send(GstElement *rtpsession, GstBuffer *pBuffer,
                                gboolean early_rtcp, gpointer udata)
{
  GstRTCPBuffer rtcp_buffer = GST_RTCP_BUFFER_INIT;
  GstRTCPPacket rtcp_packet ;
  GstRTCPPacket first_rtcp_packet ;
  guint         num_rtcp_pkts = 0;
  guint         uiIndex = 0;
  Client        *src_client = (Client *)udata;
  HmpFrameBin   *pFrameBin = src_client->m_FrameChannel->m_pHmpFrameBin;
  guint          bitrate_to_send = 0;
  guint16        tmmbr_fci_len = 0;
  guint8         *fci_data_ptr = NULL;
  guint8         *tmmbr_data_ptr = NULL;
  guint32        tmmbr_mantissa = 0;
  guint32        sender_ssrc = 0;
  static guint32  tmmbr_count  = 0;
  static timeval  prev_tv = {0,0 };
  gboolean send_tmmbr_msg = FALSE;
  src_client->m_ace_lock.lock();
  send_tmmbr_msg = src_client->send_tmmbr_message_set;
  bitrate_to_send = src_client->sent_bitrate;
  if (TRUE == send_tmmbr_msg){
    src_client->send_tmmbr_message_set = FALSE;
  }
  src_client->m_ace_lock.release();
  if(send_tmmbr_msg != TRUE){
   return TRUE;
  }
  if (NULL != pFrameBin->videoSrcPad){
    /*XGLOG_INFO("Entered on_rtcp_send () client: %s RTCP buffer: %p webrtcbin: %p",
				src_client->m_detail.m_callId.c_str(),
                                pBuffer, pFrameBin->m_webrtcbin);*/
    gst_rtcp_buffer_map (pBuffer, GST_MAP_READWRITE, &rtcp_buffer);
    gst_rtcp_buffer_get_first_packet(&rtcp_buffer,&first_rtcp_packet);
    if (GST_RTCP_TYPE_RR == gst_rtcp_packet_get_type(&first_rtcp_packet)){
      sender_ssrc = gst_rtcp_packet_rr_get_ssrc (&first_rtcp_packet);
      if((TRUE == gst_rtcp_buffer_add_packet (&rtcp_buffer,GST_RTCP_TYPE_RTPFB , &rtcp_packet))
        && (pFrameBin->rtp_video_rx_ssrc > 0)){
        /* set media ssrc (unused = 0) */
        gst_rtcp_packet_fb_set_media_ssrc (&rtcp_packet,0);
        /* set the sender ssrc i,,e our rtcp ssrc */
        gst_rtcp_packet_fb_set_sender_ssrc (&rtcp_packet,sender_ssrc);
        /* set the feedback type as TMMBR */
        gst_rtcp_packet_fb_set_type (&rtcp_packet ,GST_RTCP_RTPFB_TYPE_TMMBR);
        tmmbr_fci_len = 2;
        if(TRUE == gst_rtcp_packet_fb_set_fci_length (&rtcp_packet,tmmbr_fci_len)){
          fci_data_ptr = gst_rtcp_packet_fb_get_fci (&rtcp_packet);
          tmmbr_data_ptr = fci_data_ptr;
          GST_WRITE_UINT32_BE (tmmbr_data_ptr,pFrameBin->rtp_video_rx_ssrc);
          tmmbr_data_ptr += sizeof (guint32);
          tmmbr_mantissa = pFrameBin->tmmbr_calculate(bitrate_to_send);//Added convert tmmbr_mantissa format calculate funtion
          GST_WRITE_UINT32_BE (tmmbr_data_ptr, tmmbr_mantissa);
          XGLOG_INFO ("tmmbr_mantissa: %u  After converting to BE: %u",tmmbr_mantissa,*(guint32*)tmmbr_data_ptr);
          gst_rtcp_buffer_unmap(&rtcp_buffer);
        }
        else{
          XGLOG_ERROR ("Failed to allocate buffer for FCI information");
        }
      }else{
        XGLOG_ERROR("Failed to create RTCP buffer");
      }
    }
  }
  return TRUE;
}

unsigned int HmpFrameBin :: tmmbr_calculate(unsigned int bitrate)
{
  unsigned int bitrtmax=bitrate;
  unsigned int mantissa = 0;
  int expnt = 0;
  XGLOG_INFO("Recieved Bitrate to calculate is %u ",bitrtmax);
  while(bitrtmax > MAX_BTRT)
  {
    bitrtmax /= 2;
    ++expnt;
  }
  mantissa = bitrtmax;
  int resultant_mantissa = ((expnt << 26) | ((mantissa << 9)&(0x03fffe00)));
  return resultant_mantissa;
}

void HmpFrameBin::SendKeyFrameRequest(Client *client)
{
  GstPad *videosrcpad = NULL;
  HmpFrameBin *frameRecvBin = NULL;
  int index = 0;

  if(client && client->m_FrameChannel){
    frameRecvBin = client->m_FrameChannel->m_pHmpFrameBin;
  }
  if ((frameRecvBin) && (frameRecvBin->m_webrtcbin)){
    if (frameRecvBin->videoSrcPad){
      videosrcpad = frameRecvBin->videoSrcPad;
    }
    if(videosrcpad)
    {
      XGLOG_INFO("Sending ForceKey webrtcbin: %p videosrcpad: %p callId: %s",
            frameRecvBin->m_webrtcbin,videosrcpad, client->m_detail.m_callId.c_str());
      gst_pad_send_event(videosrcpad, gst_event_new_custom(GST_EVENT_CUSTOM_UPSTREAM,
      gst_structure_new("GstForceKeyUnit", "all-headers",G_TYPE_BOOLEAN, TRUE, NULL)));
    }else{
      XGLOG_ERROR("Failed to get video srcpad. SrcPad not yet created for webrtcbin: %p callId: %s",
                  frameRecvBin->m_webrtcbin,client->m_detail.m_callId.c_str());
    }
  }
}

gboolean HmpFrameBin::handle_sinkpad_event (GstPad *sinkPad, Client *client)
{
  GstCaps *filter = NULL;
  GstCaps *sinkPad_caps = gst_pad_query_caps (sinkPad, filter);
  gchar *sinkPad_caps_string = NULL;
  gchar *sinkPadName = gst_pad_get_name(sinkPad);
  HmpFrameBin *frameRecvBin = this;
  gboolean retVal = TRUE;
  GstCaps *caps = NULL;
  guint uiSSRC = 0;
  guint uiPayloadNum = 0;
  guint uPadType = 0;
  guint8  u8StreamId [100] = {'\0'};
  gchar *stream_id = NULL;
  MgMediaDetail sMediaDetail;
  mlineofferDetail *mlineDetails = NULL;
  GstStructure *caps_structure = NULL;
  GstPad  *srcPad = NULL;
  gchar   *encoding_name = NULL;
  ACE_Message_Block* pAmb = NULL;
  xGateHmpClientThreadMsg *pMsg = NULL;
  ACE_Time_Value tval ((time(NULL) ));
  HmpFrameChannel  *pFrameChannel = client->m_FrameChannel;
  GstElement *element  = NULL;
  gchar *srcPadName = NULL;
  HmpAudioEncoderBin *AudioEncoderBin = NULL;
  GstPadLinkReturn isPadLinked;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  if(!frameRecvBin) {
    XGLOG_ERROR( "handle_sinkpad_event failed: %s. Invalid HmpFrameBin pointer", sinkPadName);
    goto return_from_func;
  }

  if(!sinkPad_caps){
    XGLOG_ERROR("==== sinkPad caps is NULL");
    goto return_from_func;
  }else{
    sinkPad_caps_string = gst_caps_to_string (sinkPad_caps);
    if(sinkPad_caps_string){
       XGLOG_INFO("Sink Pad current caps: %s",sinkPad_caps_string);
    }else{
     XGLOG_INFO("Sink Pad caps after converting to string is null ");
     goto return_from_func;
    }
  }
   /* get the ssrc and payload number to identify the RTP stream for which
    * this function is called
    */
  caps_structure = gst_caps_get_structure (sinkPad_caps, 0);
  if (caps_structure) {
    gst_structure_get (caps_structure,
                "ssrc", G_TYPE_UINT, &uiSSRC,
                "payload", G_TYPE_INT, &uiPayloadNum,
                "encoding-name", G_TYPE_STRING, &encoding_name,
                 NULL);
  }else {
    XGLOG_ERROR ("Failed to retrieve SSRC, payload number from caps ");
    goto return_from_func;
  }

  if(strstr(sinkPad_caps_string, "media=(string)audio")){
    uPadType = MEDIA_CAPS_FOR_AUDIO;
    sprintf (u8StreamId, "%s_Audio_%u_%u", sinkPadName, uiSSRC, uiPayloadNum);
  } else if(strstr(sinkPad_caps_string, "media=(string)video")){
    uPadType = MEDIA_CAPS_FOR_VIDEO;
    sprintf (u8StreamId, "%s_Video_%u_%u", sinkPadName, uiSSRC, uiPayloadNum);
  }

#ifdef AUDIO_MCU_ENABLED
  if(uPadType == MEDIA_CAPS_FOR_AUDIO){
    AudioEncoderBin = client->m_FrameChannel->m_pHmpAudioEncoderBin;
    if(!AudioEncoderBin){
      XGLOG_ERROR( "Failed to get EncoderBin for client:%s",client->m_detail.m_callId.c_str());
      retVal = FALSE;
      goto return_from_func;
    }
    srcPad = AudioEncoderBin->m_ghostRtpSrcPad;
    srcPadName = gst_pad_get_name(srcPad);
    XGLOG_INFO("Linking EncodeBin ghostpad: %p srcPadName: %s to webrtc sink pad: %p name: %s",
                                       srcPad, srcPadName, sinkPad, sinkPadName);
    isPadLinked = gst_pad_link(srcPad, sinkPad);
    if(isPadLinked != GST_PAD_LINK_OK) {
      XGLOG_ERROR( "Linking encodeBin ghost_pad with webrtcbin sinkpad: %s failed ! error: %d",
                     sinkPadName, isPadLinked);
      retVal = FALSE;
    } else {
      XGLOG_INFO( "encodeBin ghost_pad with webrtcbin sinkpad: %s success", sinkPadName);
    }
  }
#endif
  for (auto itr = client->m_mlineOffer_map.begin(); itr != client->m_mlineOffer_map.end(); itr++){
    mlineDetails = (mlineofferDetail*)itr->second;
    if (mlineDetails->audioOfferDetails.uiTxSSRCNum == uiSSRC){
      mlineDetails->audioOfferDetails.sinkPadName = sinkPadName;
      mlineDetails->audioOfferDetails.padType = SINK_PAD_TYPE_AUDIO;
      mlineDetails->audioOfferDetails.sinkPad = sinkPad;
      if (encoding_name){
        strcpy (mlineDetails->audioOfferDetails.encoding_name, encoding_name);
      }
      break;
    }else if (mlineDetails->videoOfferDetails.uiTxSSRCNum == uiSSRC){
      mlineDetails->videoOfferDetails.sinkPadName = sinkPadName;
      mlineDetails->videoOfferDetails.sinkPad = sinkPad;
      mlineDetails->videoOfferDetails.padType = SINK_PAD_TYPE_VIDEO;
      sMediaDetail.call_id =  mlineDetails->callId_offer_generated;
      if (encoding_name){
        strcpy (mlineDetails->videoOfferDetails.encoding_name, encoding_name);
      }
      break;
    }else{
       mlineDetails = NULL;
    }
  }
  /* check if mlineDetails are present. If there is delay to get the sinkpad event and
   * if client present in offer details is disconnected, then offer details may be deleted
   * from map before the sink pad is received from webrtc
   */
  if (!mlineDetails){
    GstWebRTCRTPTransceiver *transceiver = NULL;
    guint                    transceiver_direction = 0;
    g_object_get (sinkPad, "transceiver", &transceiver, NULL);
    if (transceiver){
      g_object_get (transceiver, "direction",
                   &transceiver_direction, NULL);
      XGLOG_ERROR ("client: %s mlineDetails not found in map. current map size: %d trans_direction: %u",
                   client->m_detail.m_callId.c_str(), client->m_mlineOffer_map.size(), transceiver_direction);
      if (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDONLY){
        XGLOG_INFO ("Inactivating transceiver direction for sink pad: %s", sinkPadName);
        g_object_set (transceiver, "direction",
               GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_INACTIVE, NULL);
        gst_object_unref (transceiver);
      }
    }
    goto return_from_func;
  }

  /* Get the element pointer of rtpfunnel so that we can get the running time
  * of the pad and
  */
  element = gst_bin_get_by_name (GST_BIN (pFrameChannel->m_pipeline), "rtpfunnel");
  if (element){
    GstClockTime clocktime = gst_element_get_current_running_time (element);
    gchar *element_name = gst_element_get_name (element);
    XGLOG_INFO ("got the clock time for pad : %s from element: %s element: %p clocktime: %lld",
              sinkPadName, element_name, element, clocktime);
    g_free (element_name);
    gst_object_unref (element);
  }else {
    XGLOG_ERROR ("Failed to get element from bin");
  }

  if ((uPadType == MEDIA_CAPS_FOR_AUDIO) && (mlineDetails->audioOfferDetails.m_appsrc))
    mlineDetails->audioOfferDetails.pPeerElement = mlineDetails->audioOfferDetails.m_appsrc;
  else if (uPadType == MEDIA_CAPS_FOR_AUDIO)
    mlineDetails->audioOfferDetails.pPeerElement = element;

  if(uPadType == MEDIA_CAPS_FOR_VIDEO)
    mlineDetails->videoOfferDetails.pPeerElement = element;

  mlineDetails->audioOfferDetails.pPeerElement = element;

  pMsg = new xGateHmpClientThreadMsg ();
  tval += ACE_Time_Value(0,1);
  strcpy (pMsg->ucName, sinkPadName);
  pMsg->m_msgType = XGATE_CLIENT_SEND_EVENT_SINK_PAD;
  pMsg->param3 = malloc (sizeof(guint));
  if (uPadType == MEDIA_CAPS_FOR_AUDIO){
    pMsg->param2 = (void *) &(mlineDetails->audioOfferDetails);
    *((guint *)pMsg->param3) = MEDIA_CAPS_FOR_AUDIO;
    pMsg->str_param = mlineDetails->callId_offer_generated;
    pMsg->param1 = NULL;
  }else{
    pMsg->param2 = (void *) &(mlineDetails->videoOfferDetails);
    *((guint *)pMsg->param3) = MEDIA_CAPS_FOR_VIDEO;
    pMsg->str_param = mlineDetails->callId_offer_generated;
    pMsg->param1 = NULL;
  }
  pMsg->pClient = client;

  if (pSFUController->m_ThreadPool_info.m_event_pool) {
    GError *error = NULL;
    g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, &error);
    if (error != NULL){
      XGLOG_ERROR ("Failed to push event to thread pool");
      g_error_free (error);
    }
  }else{
    XGLOG_ERROR ("event pool thread is null... can't push event");
  }


 /* If call Type is conference and a new client joins, it may happen that
  * other clients may lose initial frames due to delay in connection establishment
  * For example if client 3 joins the conference when client 1,2 are already in
  * conference, first client 3's offer will be accepted and then invite to client 1,2
  * will be sent intimating that a new client has joined.
  *
  * In above case client 3 will be in ready state to receive data from 1,2 but 1,2 are
  * not yet ready to receive data from client 3 and initial pkts may not be decoded
  * To avoid this request client 3 to send Key frame as soon as client 1,2 starts receiving
  * data
  */
  if((EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO == client->m_detail.m_callType)
    && (uPadType == MEDIA_CAPS_FOR_VIDEO )){
    xGateHmpConference *pxGateHmpConfObj = NULL;
    pxGateHmpConfObj = pSFUController->find_conference_entry(client->m_detail.m_mgresourceId);
    if(!pxGateHmpConfObj){
      XGLOG_ERROR("Failed to find entry in conference");
      goto return_from_func;
    }
    if (!sMediaDetail.call_id.empty()){
      sMediaDetail.mgresource_id = client->m_detail.m_mgresourceId;
      sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
      sMediaDetail.joiner_type = client->m_detail.m_joinerType;
      XGLOG_INFO( "Call Type is conference video.. sending key frame request to client: %s",
                  sMediaDetail.call_id.c_str());
      pSFUController->send_msg_sfu_client(&sMediaDetail,client->m_detail,
                                        EN_XGATE_MG_MEDIA_REQUEST_KEY_FRAME);
    }
  }
  retVal = TRUE;
return_from_func:
  if (sinkPad_caps){
    gst_caps_unref (sinkPad_caps);
  }
  if (sinkPad_caps_string){
    g_free(sinkPad_caps_string);
  }
  if (sinkPadName){
    g_free(sinkPadName);
  }
  if (encoding_name){
    g_free (encoding_name);
  }
  if (srcPadName){
    g_free(srcPadName);
  }
  return retVal;
}

gboolean HmpFrameBin::add_audiomixer_sinkpad(Client *client,Client *src_client, guint uiSSRC)
{
  guint8     u8StreamId [100] = {'\0'};
  gchar      *padName = NULL;
  gchar      *mixerName = NULL;
  GstElement *audio_appsrc= NULL;
  gchar       audio_appsrc_name[100] = {'\0'};
  GstElement  *element  = NULL;
  GstPad      *appsrcPad = NULL;
  GstCaps     *audio_caps = NULL;
  GstPadLinkReturn isPadLinked;
  GstElement  *mixer = NULL;
  GstPad      *mixer_sinkpad = NULL;
  AUDIO_MIXER_INPUT_INFO *audio_mixer_inp_info = nullptr;
  HmpFrameChannel  *pFrameChannel = client->m_FrameChannel;
  HmpAudioEncoderBin *AudioEncoderBin = client->m_FrameChannel->m_pHmpAudioEncoderBin;

  XGLOG_INFO ("Entered add_audiomixer_sinkpad() client: %s src_client: %s",
                      client->m_detail.m_callId.c_str(), src_client->m_detail.m_callId.c_str());
  if(!AudioEncoderBin){
    XGLOG_ERROR( "Failed to get EncoderBin for client:%s",client->m_detail.m_callId.c_str());
    return FALSE;
  }
  g_mutex_lock (&AudioEncoderBin->m_mixer_lock);
  /*check if sink pad is created already */
  for (guint32 uiIndex = 0; uiIndex < AudioEncoderBin->m_mixer_input_info.size(); uiIndex++){
    audio_mixer_inp_info = AudioEncoderBin->m_mixer_input_info[uiIndex];
    XGLOG_INFO ("verifying audio_mixer_inp_info->src_callId: %s  src_client->m_detail.m_callId: %s",
               audio_mixer_inp_info->src_callId.c_str(), src_client->m_detail.m_callId.c_str());
    if (audio_mixer_inp_info->src_callId == src_client->m_detail.m_callId){
       XGLOG_INFO("Mixer sink pad already exists for callId: %s", src_client->m_detail.m_callId.c_str());
       g_mutex_unlock (&AudioEncoderBin->m_mixer_lock);
       return TRUE;
     }else {
        /* check if callId is present in client Map. If it's not present, client would have got
         * disconnected. in that case, reuse the sink pad instead of creating  it
         */
        SFUController *pSFUController = xGateSFUtil::getSFUController();
        xGateHmpConference *pHmpConference
             = pSFUController->find_conference_entry(src_client->m_detail.m_mgresourceId);
        if(!pHmpConference){
          XGLOG_ERROR("Failed to find entry in conference");
        }else{
          Client *existing_client = pHmpConference->get_participant_entry (audio_mixer_inp_info->src_callId);
           if(!existing_client ){
              XGLOG_INFO ("client: %s does not exists .. re-using the mixer sinkpad for new client: %s",
                    audio_mixer_inp_info->src_callId.c_str(), src_client->m_detail.m_callId.c_str());
              audio_mixer_inp_info->src_callId.clear();
              audio_mixer_inp_info->src_callId = src_client->m_detail.m_callId;
              g_mutex_unlock (&AudioEncoderBin->m_mixer_lock);
              return TRUE;
            }
         }
     }
  }

  {
    mixer_sinkpad = AudioEncoderBin->Generate_new_Audiomixer_sinkpad();
    if(!mixer_sinkpad){
      XGLOG_ERROR("Failed to get AudioMixer SinkPad");
      g_mutex_unlock (&AudioEncoderBin->m_mixer_lock);
      return FALSE;
    }
    mixer = AudioEncoderBin->m_audiomixer;
    padName = gst_pad_get_name (mixer_sinkpad);
    mixerName = gst_element_get_name (AudioEncoderBin->m_audiomixer);
    if (padName){
      sprintf (u8StreamId, "%s_%s_pad", mixerName,padName);
    } else{
      sprintf (u8StreamId, "audio_mixer_sink_%p_pad", mixer_sinkpad);
    }
    XGLOG_INFO( "Mixer Sink Pad: %p created for sink pad of client: %s",
                mixer_sinkpad,src_client->m_detail.m_callId.c_str());
#if 0
    sprintf (audio_appsrc_name, "%s%u","Appsrc_Audio_",uiSSRC );
    XGLOG_INFO( "create_audio_appsrc_element: %s for client: %s ",
                                  audio_appsrc_name,
                                  client->m_detail.m_callId.c_str());
    audio_appsrc  = gst_element_factory_make("appsrc",audio_appsrc_name);
    if (!audio_appsrc)
    {
      XGLOG_ERROR( "create_link_audio_appsrc_element failed while creating audio_appsrc");
      return FALSE;
    }
#endif
    if (audio_appsrc) {
      audio_caps = gst_caps_from_string (CAPS_STR_FOR_AUDIODEC);
      g_object_set(G_OBJECT(audio_appsrc),
                 "caps", audio_caps,
                 "is-live", (gboolean) TRUE,
                 "format", GST_FORMAT_TIME,
                 "do-timestamp" , TRUE,
                 "emit-signals", false,
                 "max-buffers", APPSRC_AUDIO_MAX_BUFFERS,
                 NULL);
      gst_bin_add_many (AudioEncoderBin->m_bin, audio_appsrc, NULL);
      gst_caps_unref (audio_caps);

      appsrcPad = gst_element_get_static_pad(audio_appsrc, "src");
      isPadLinked = gst_pad_link(appsrcPad, mixer_sinkpad);
      if(isPadLinked != GST_PAD_LINK_OK) {
        XGLOG_ERROR( "Linking audio_appsrcPad with mixer_sinkpad: failed !");
      }
      gst_element_sync_state_with_parent(audio_appsrc);
    }
    /*
     * create stream start event and push it to sink pad
     */
    gst_pad_activate_mode (GST_PAD (mixer_sinkpad), GST_PAD_MODE_PUSH , TRUE);
    XGLOG_INFO ("stream_id created for pad: %p is %s ", mixer_sinkpad, u8StreamId);

   /* Get the element pointer of rtpfunnel so that we can get the running time
     * of the pad and
     */
    element = gst_bin_get_by_name (GST_BIN (pFrameChannel->m_pipeline), "rtpfunnel");
    if (element){
      GstClockTime clocktime = gst_element_get_current_running_time (element);
      gchar *element_name = gst_element_get_name (element);
      gchar *sinkPadName = gst_pad_get_name(mixer_sinkpad);
      XGLOG_INFO ("got the clock time for pad : %s from element: %s element: %p clocktime: %ld",
              (sinkPadName == NULL) ? "" : sinkPadName, element_name, element, clocktime);
      g_free (element_name);
      gst_object_unref (element);
      if (sinkPadName)
        g_free (sinkPadName);
    }else {
      XGLOG_ERROR ("Failed to get element from AudioEncbin");
    }
    audio_mixer_inp_info = new (AUDIO_MIXER_INPUT_INFO);
    if (audio_appsrc){
      audio_mixer_inp_info->pPeerElement = audio_appsrc;
      audio_mixer_inp_info->m_appsrc = audio_appsrc;
      element = audio_appsrc;
    } else {
      audio_mixer_inp_info->pPeerElement = element;
      audio_mixer_inp_info->m_appsrc = NULL;
    }
    audio_mixer_inp_info->pPeerElement = element;
    audio_mixer_inp_info->sink_pad = mixer_sinkpad;
    audio_mixer_inp_info->src_callId = src_client->m_detail.m_callId;
    audio_mixer_inp_info->bStartEventSent = FALSE;
    AudioEncoderBin->m_mixer_input_info.push_back(audio_mixer_inp_info);

    g_mutex_unlock (&AudioEncoderBin->m_mixer_lock);

    if (NULL == audio_mixer_inp_info->m_appsrc){
      if (!AudioEncoderBin->send_stream_start_event (mixer_sinkpad,
                                     STREAM_START_EVENT_PAD, u8StreamId)){
        XGLOG_ERROR ("Failed to send stream start event");
      }
      /*set the mixer sink pad caps */
      GstCaps *pad_caps = gst_caps_from_string (CAPS_STR_FOR_AUDIOENC);
      if (pad_caps){
       GstEvent *caps_event = gst_event_new_caps (pad_caps);
       if (FALSE == gst_pad_send_event (mixer_sinkpad, caps_event)){
         XGLOG_ERROR ("Failed to send caps Event for mixer sink pad");
       }
       gst_caps_unref (pad_caps);
     }
     if (!AudioEncoderBin->send_segment_event (mixer_sinkpad, SEGMENT_EVENT_TIME_PAD)){
       XGLOG_ERROR ("Failed to send segment start event");
     }
    }
  }
  audio_mixer_inp_info->bStartEventSent = TRUE;

  pFrameChannel->draw_dot_file ();

  if (padName){
    g_free(padName);
  }

  if (mixerName){
    g_free(mixerName);
  }
  return TRUE;
}

void HmpAudioEncoderBin::remove_audiomixer_sinkpad(Client *pClient, string &callId)
{
  HmpFrameChannel *pFrameChannel = pClient->m_FrameChannel;
  HmpAudioEncoderBin *pAudEncBin = pFrameChannel->m_pHmpAudioEncoderBin;
  AUDIO_MIXER_INPUT_INFO *audio_mixer_inp_info = nullptr;
  guint32 uiIndex = 0;
  if (pAudEncBin){
    GstPad  *mixer_pad = NULL;
    GstPad  *appsrc_pad = NULL;

    for (uiIndex = 0; uiIndex < pAudEncBin->m_mixer_input_info.size(); uiIndex++){
      XGLOG_INFO ("comparing audio_mixer_inp_info->src_callId: %s  with callId: %s",
               pAudEncBin->m_mixer_input_info[uiIndex]->src_callId.c_str(), callId.c_str());
      if (pAudEncBin->m_mixer_input_info[uiIndex]->src_callId == callId){
        XGLOG_INFO("Matched Mixer sink pad for callId: %s", callId.c_str());
        audio_mixer_inp_info = pAudEncBin->m_mixer_input_info[uiIndex];
        break;
      }
    }
    if (audio_mixer_inp_info && audio_mixer_inp_info->m_appsrc){
      appsrc_pad = gst_element_get_static_pad(audio_mixer_inp_info->m_appsrc, "src");
    }
    if(audio_mixer_inp_info)
      mixer_pad = audio_mixer_inp_info->sink_pad;
    if (mixer_pad && appsrc_pad) {
      XGLOG_INFO ("Unlinking mixer sink_pad and appsrc_pad for call_id: %s",
                         callId.c_str ());
      gst_pad_unlink (appsrc_pad, mixer_pad);
    }
    if (mixer_pad){
      gst_pad_set_active (mixer_pad, FALSE);
      gst_element_release_request_pad (pAudEncBin->m_audiomixer, mixer_pad);
      gst_object_unref (mixer_pad);
    }
    if (audio_mixer_inp_info && audio_mixer_inp_info->m_appsrc){
      gst_object_ref (audio_mixer_inp_info->m_appsrc);
      gst_app_src_end_of_stream (GST_APP_SRC_CAST (audio_mixer_inp_info->m_appsrc));
      gst_bin_remove (pAudEncBin->m_bin, audio_mixer_inp_info->m_appsrc);
      gst_element_set_state (audio_mixer_inp_info->m_appsrc, GST_STATE_NULL);
      gst_object_unref (audio_mixer_inp_info->m_appsrc);
      audio_mixer_inp_info->m_appsrc = NULL;
      pAudEncBin->m_mixer_input_info.erase (pAudEncBin->m_mixer_input_info.begin() + uiIndex);
    }
    gst_element_sync_state_with_parent(pAudEncBin->m_audiomixer);
    pFrameChannel->draw_dot_file ();
  }
}

gboolean HmpFrameBin::handle_srcpad_event (GstPad *srcPad, Client *src_client)
{
  gchar         *srcPad_caps_string = NULL;
  guint8        u8StreamId[100] = {'\0'};
  GstCaps       *caps = NULL;
  int           uPadType = 0;
  guint         ssrc = 0;
  gint          payload = 0;
  gchar         *encoding_name = NULL;
  GstStructure  *caps_structure = NULL;
  string        transceiverPad = "";
  gchar         *srcPadName = gst_pad_get_name(srcPad);
  GstCaps       *srcPad_caps = gst_pad_get_current_caps (srcPad);
  HmpFrameBin   *frameRecvBin = this;
  gboolean      retVal = FALSE;
  gulong        probeId = 0;

  if(!frameRecvBin) {
    XGLOG_ERROR("handle_srcpad_event () Invalid HmpFrameBin pointer", srcPadName);
    goto return_from_func;
  }

 /* Get the type of pad detected, audio or video. we can get this by
  * by verifying the remote sdp. pad name from webrtc plugin is in format
  * src_media-id_rid number. if mid:0 describes audio caps then src_0 is
  * audio pad ,on the other side if mid:0 describes video caps then src_0 is
  * video
  */
  if (!srcPad_caps){
    XGLOG_ERROR("handle_srcpad_event () caps not available for src pad", srcPadName);
    goto return_from_func;
  }
  caps_structure = gst_caps_get_structure (srcPad_caps, 0);
  if (caps_structure) {
    gst_structure_get (caps_structure,
                "ssrc", G_TYPE_UINT, &ssrc,
                "payload", G_TYPE_INT,&payload,
                 "encoding-name",G_TYPE_STRING, &encoding_name,
                 NULL);
  }
  transceiverPad = srcPadName;

  srcPad_caps_string = gst_caps_to_string (srcPad_caps);
  if(srcPad_caps_string){
    XGLOG_INFO("Src Pad current caps: %s",srcPad_caps_string);
    if(strstr(srcPad_caps_string, "media=(string)video")){
      uPadType = MEDIA_CAPS_FOR_VIDEO;
      if ((src_client->m_detail.m_joinerType == EN_JOINER_TYPE_SCREENSHARE)
        && (!src_client->m_detail.m_screenshare_id.empty())){
         transceiverPad +="_screenshare_";
       }else {
         transceiverPad += "_Video_";
       }
    }else if(strstr(srcPad_caps_string, "media=(string)audio")){
      uPadType = MEDIA_CAPS_FOR_AUDIO;
      transceiverPad += "_Audio_";
    }else{
      XGLOG_ERROR( "Could not find audio or video Pad caps in SRC pad");
      g_free(srcPadName);
      g_free(srcPad_caps_string);
      return NULL;
    }
  }
  transceiverPad += encoding_name;
  transceiverPad += "_";
  transceiverPad += to_string (ssrc);
  transceiverPad += "_";
  transceiverPad += to_string (payload);
#ifdef AUDIO_MCU_ENABLED
  if (uPadType == MEDIA_CAPS_FOR_AUDIO){
    GstPadLinkReturn isPadLinked;
    HmpAudioDecoderBin *Decoderbin = src_client->m_FrameChannel->m_pHmpAudioDecoderBin;
    if(!Decoderbin){
      XGLOG_WARN( "DecoderBin not yet created for client: %s ... creating decoder bin ",
         src_client->m_detail.m_callId.c_str());
       unsigned int codec = src_client->m_detail.audioDetail.m_codec;
      if (TRUE != src_client->m_FrameChannel->construct_decoder_bin (EN_BIN_VID_DTLS_RECV, codec, src_client)){
        XGLOG_ERROR( "Failed to create DecoderBin for client: %s ",
         src_client->m_detail.m_callId.c_str());
        return FALSE;
      }
      Decoderbin = src_client->m_FrameChannel->m_pHmpAudioDecoderBin;
      if (!Decoderbin)
         return FALSE;
    }
    GstElement *capsfilter = Decoderbin->m_capsfilter;
    if(!capsfilter){
      XGLOG_ERROR( "CapsFilter not created for client:%s",src_client->m_detail.m_callId.c_str());
      return FALSE;
    }
    GstPad *capsSrcPad = gst_element_get_static_pad(capsfilter, "src");
    if(!capsSrcPad){
      XGLOG_ERROR( "Not able to get capsfilterSrcPad for client:%s",src_client->m_detail.m_callId.c_str());
      return FALSE;
    }
    GstPad *sinkPad = Decoderbin->m_ghostRtpSinkPad;
    if(!sinkPad){
      XGLOG_ERROR( "DecoderBin GhostSinkpad not created for client:%s",src_client->m_detail.m_callId.c_str());
      return FALSE;
    }
    probeId = gst_pad_add_probe (srcPad,
                              (GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_PUSH ),
                              handle_audioRTPPadProbeCallback, src_client, NULL);
    sprintf (u8StreamId, "webrtc_%s_audio_rtp",srcPadName );
    if (!Decoderbin->send_stream_start_event (sinkPad,
                                     STREAM_START_EVENT_PAD, u8StreamId)){
       XGLOG_ERROR ("Failed to send stream start event");
     }
     if (srcPad_caps){
       GstEvent *caps_event = gst_event_new_caps (srcPad_caps);
       if (FALSE == gst_pad_send_event (sinkPad, caps_event)){
         XGLOG_ERROR ("Failed to send caps Event to decoder bin");
       }
     }
     if (!Decoderbin->send_segment_event (sinkPad, SEGMENT_EVENT_TIME_PAD)){
       XGLOG_ERROR ("Failed to send segment start event");
     }

    frameRecvBin->audioSrcPad = capsSrcPad;
    frameRecvBin->audioSrcPadName = transceiverPad;
    probeId = gst_pad_add_probe (capsSrcPad,
                              (GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_PUSH ),
                              handle_PadProbeCallback, src_client, NULL);
   /* Send add audio mixer pad event to all clients in conference*/
     SFUController *pSFUController = xGateSFUtil::getSFUController();
     xGateHmpConference *pHmpConference
          = pSFUController->find_conference_entry(src_client->m_detail.m_mgresourceId);

     if(!pHmpConference){
        XGLOG_ERROR("Failed to find entry in conference");
        goto return_from_func;
      }
  #ifdef USE_GST_AUDIO_MIXER_PLUGIN
      for (auto itr = pHmpConference->m_clientMap.begin ();
                itr != pHmpConference->m_clientMap.end (); ++itr){
        Client *dst_client = itr->second;
        if (dst_client && (dst_client != src_client) &&
           (dst_client->m_detail.m_screenshare_id != src_client->m_detail.m_callId) &&
           (0 != strcmp (dst_client->m_detail.m_callId.c_str(), EMPTY_DUMMY_CALLID))){
             ACE_Message_Block* pAmb = NULL;
             ACE_Time_Value tval ((time(NULL) ));
             xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg ();
             tval += ACE_Time_Value(0,1);
             pMsg->m_msgType = XGATE_CLIENT_ADD_AUDIO_MIXER_PAD;
             pMsg->param1 = static_cast <void*> (src_client);
             XGLOG_INFO ("ProbeCallback (): Posting ADD_AUDIO_MIXER_PAD to client: %s For data from src_client: %s",
                          dst_client->m_detail.m_callId.c_str(),
                          src_client->m_detail.m_callId.c_str());
             pMsg->pClient = dst_client;
             if (pSFUController->m_ThreadPool_info.m_event_pool) {
               GError *error = NULL;
               g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, &error);
               if (error != NULL){
                 XGLOG_ERROR ("Failed to push event to thread pool");
                 g_error_free (error);
               }
             } else{
               XGLOG_ERROR ("event pool thread is null... can't push event");
             }
         }
      }
   #endif
  }
  else if (uPadType == MEDIA_CAPS_FOR_VIDEO){
    frameRecvBin->videoSrcPad = srcPad;
    frameRecvBin->videoSrcPadName = transceiverPad;
    frameRecvBin->retrieved_videossrc = ssrc;
    probeId = gst_pad_add_probe (srcPad,
                              (GST_PAD_PROBE_TYPE_BUFFER_LIST | GST_PAD_PROBE_TYPE_PUSH ),
                              handle_PadProbeCallback, src_client, NULL);
    // check if all the participants existing in call are muted. in that
    // case, as per requirements, the last connected participant video should
    // be sent as active speaker . The last connected participant should receive
    // previous last connected partcipant's video
    SFUController *pSFUController = xGateSFUtil::getSFUController();
    xGateHmpConference *pHmpConference = NULL;
    if (!src_client->m_detail.m_callId.c_str() || !src_client->m_detail.m_mgresourceId.c_str()){
       XGLOG_ERROR("Client details are invalid calliD: %s or resource Id: %s is null",
                     src_client->m_detail.m_callId.c_str(), src_client->m_detail.m_mgresourceId.c_str());
       return EN_XGATE_STATUS_ERROR;
    }
    pHmpConference = pSFUController->find_conference_entry(src_client->m_detail.m_mgresourceId);
    if(!pHmpConference){
      XGLOG_ERROR("Failed to find entry in conference");
      return EN_XGATE_STATUS_ERROR;
    }
    pHmpConference->find_initial_active_spkr_client ();
  }
#else
  if (uPadType == MEDIA_CAPS_FOR_AUDIO){
    frameRecvBin->audioSrcPad = srcPad;
    frameRecvBin->audioSrcPadName = transceiverPad;
    frameRecvBin->retrieved_audiossrc = ssrc;
  }
  else if (uPadType == MEDIA_CAPS_FOR_VIDEO){
    frameRecvBin->videoSrcPad = srcPad;
    frameRecvBin->videoSrcPadName = transceiverPad;
    frameRecvBin->retrieved_videossrc = ssrc;
  }
  probeId = gst_pad_add_probe (srcPad,
                              (GST_PAD_PROBE_TYPE_BUFFER_LIST | GST_PAD_PROBE_TYPE_PUSH ),
                              handle_PadProbeCallback, src_client, NULL);
#endif
  XGLOG_INFO ("Probe Id generated is : %lu transceiverPad name: %s ",
                      probeId, transceiverPad.c_str());
  transceiverPad.clear();
  if (encoding_name){
    g_free (encoding_name);
  }
  retVal = TRUE;

return_from_func:

  if (srcPadName){
    g_free (srcPadName);
  }
  if(srcPad_caps){
    gst_caps_unref(srcPad_caps);
  }
  if (srcPad_caps_string){
    g_free(srcPad_caps_string);
  }
  return retVal;
}

GstPadProbeReturn HmpFrameBin::handle_PadProbeCallback (GstPad * pad,
                                                        GstPadProbeInfo * ProbeInfo,
                                                        gpointer user_data)
{
  gchar *element_name = NULL;
  HmpFrameBin   *frameBin = NULL;
  SFUController *pSFUController = NULL;
  GstBufferList   *list = NULL;
  GstBuffer       *buffer = NULL;
  GstBufferList   *audio_list = NULL;
  ACE_Message_Block* pAmb = NULL;
  xGateHmpClientThreadMsg *pMsg = NULL;
  Client *src_client = (Client *)user_data;
  Client *dst_client = NULL;
  ACE_Time_Value tval ((time(NULL) ));
  pSFUController = xGateSFUtil::getSFUController();

  XGLOG_TRACE ("handle_PadProbeCallback () Received data from client: %s joiner_type: %d ",
                     src_client->m_detail.m_callId.c_str(), src_client->m_detail.m_joinerType);
  g_return_if_fail (GST_IS_PAD (pad));

  if((ProbeInfo->type & GST_PAD_PROBE_TYPE_BUFFER_LIST)
            == GST_PAD_PROBE_TYPE_BUFFER_LIST){
    list = gst_pad_probe_info_get_buffer_list (ProbeInfo);
    if (!list){
      XGLOG_ERROR ("Buffer list is empty ");
      goto return_from_func;
    }
  }else if ((ProbeInfo->type & GST_PAD_PROBE_TYPE_BUFFER )
           == GST_PAD_PROBE_TYPE_BUFFER){
    buffer = gst_pad_probe_info_get_buffer (ProbeInfo);
    if (!buffer){
      XGLOG_ERROR ("Buffer is empty ");
      goto return_from_func;
    }
  }else {
    XGLOG_ERROR ("unknown Probe type ");
    goto return_from_func;
  }

  if (!src_client){
    XGLOG_ERROR ("could not get src_client pointer");
    goto free_buffer;
  }

  if (TRUE == src_client->m_StopSendRecvData){
    XGLOG_INFO ("client is stopping freeing buffer and returning");
    goto free_buffer;
  }

  frameBin = src_client->m_FrameChannel->m_pHmpFrameBin;
  if (frameBin->audioSrcPad == pad){
    element_name = frameBin->audioSrcPadName.c_str();
  } else if (frameBin->videoSrcPad == pad){
     element_name = frameBin->videoSrcPadName.c_str();
  } else {
    XGLOG_ERROR ("Pad: %p received not matching with stored audio: %p or video: %p pad",
            pad , frameBin->audioSrcPad , frameBin->videoSrcPad);
    goto free_buffer;
  }
  //post the extracted buffer list and Client object to mirror thread
  /* Post message to thread if thread is created , otherwise post the
   * buffer to sender thread. Need to check if RCV thread is required
   */

  if (frameBin->videoSrcPad == pad){
    XGLOG_TRACE ("client: %s RCVD video buffer_list: %p  length: %d",
            src_client->m_detail.m_callId.c_str(), list, gst_buffer_list_length(list));
  }
  if ((pSFUController->m_ThreadPool_info.m_rx_data_pool) || (src_client->rcv_data_thrId)) {
      pMsg = new xGateHmpClientThreadMsg ();
      tval += ACE_Time_Value(0,1);
      strcpy (pMsg->ucName, element_name);
      pMsg->m_msgType = XGATE_CLIENT_RCVD_RTP_DATA;
      if(list)
        pMsg->param2 = (void *) list;
      else if(buffer)
        pMsg->param2 = (void *) buffer;
      pMsg->param1 = src_client;

      if (pSFUController->m_ThreadPool_info.m_rx_data_pool) {
        GError  *error = NULL;
        pMsg->pClient = src_client;
        g_mutex_lock (&src_client->m_data_process_lock);

        if (frameBin->audioSrcPad == pad){
          src_client->ullNextAudioMsgNum++;
          pMsg->ullMsgNumber = src_client->ullNextAudioMsgNum;
          XGLOG_TRACE ("Pushing Audio buffer: %p MsgNumber: %llu client: %s", pMsg->param2,
                          pMsg->ullMsgNumber,
                          src_client->m_detail.m_callId.c_str());

        } else if (frameBin->videoSrcPad == pad){
          src_client->ullNextVideoMsgNum++;
          pMsg->ullMsgNumber = src_client->ullNextVideoMsgNum;
          XGLOG_TRACE ("Pushing Video buffer list: %p MsgNumber: %llu client: %s", pMsg->param2,
                          pMsg->ullMsgNumber,
                          src_client->m_detail.m_callId.c_str());
        }
        g_mutex_unlock (&src_client->m_data_process_lock);

        g_thread_pool_push (pSFUController->m_ThreadPool_info.m_rx_data_pool, pMsg, &error);
        if (error != NULL){
          XGLOG_ERROR ("Failed to push event to thread pool");
          if (frameBin->audioSrcPad == pad){
            src_client->ullNextAudioMsgNum--;
          } else if (frameBin->videoSrcPad == pad){
            src_client->ullNextVideoMsgNum--;
          }
          g_error_free (error);
        }
      } else if (src_client->rcv_data_msg_q) {
        pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
        if(!src_client->rcv_data_msg_q->enqueue_prio(pAmb, &tval)){
          XGLOG_ERROR("Failed to push RTP data from padProbe callback to Client thread");
        }
      } else {
        if(list)
          gst_buffer_list_unref (list);
        else if(buffer)
          gst_buffer_unref (buffer);
      }
    goto return_from_func;
  }else {
    if(list)
      frameBin->on_new_data_rcvd_srcPad(src_client, element_name, list);
    else if(buffer )
      frameBin->on_new_data_rcvd_srcPad(src_client, element_name, buffer);
    goto return_from_func;
  }

free_buffer:
   if (list)
     gst_buffer_list_unref (list);
   else if (buffer)
     gst_buffer_unref (buffer);
return_from_func:
  return GST_PAD_PROBE_HANDLED ;
}

GstPadProbeReturn HmpFrameBin::handle_audioRTPPadProbeCallback(GstPad * pad,
                                                        GstPadProbeInfo * ProbeInfo,
                                                        gpointer user_data)
{
  HmpFrameBin   *frameBin = NULL;
  SFUController *pSFUController = NULL;
  GstBufferList   *list = NULL;
  GstBuffer       *buffer = NULL;
  void            *param = NULL;
  Client *src_client = (Client *)user_data;
  pSFUController = xGateSFUtil::getSFUController();

  g_return_if_fail (GST_IS_PAD (pad));

  if ((ProbeInfo->type & GST_PAD_PROBE_TYPE_BUFFER )
           == GST_PAD_PROBE_TYPE_BUFFER){
    buffer = gst_pad_probe_info_get_buffer (ProbeInfo);
    if (!buffer){
      XGLOG_ERROR ("Buffer is empty ");
      goto return_from_func;
    }
  } else if((ProbeInfo->type & GST_PAD_PROBE_TYPE_BUFFER_LIST)
            == GST_PAD_PROBE_TYPE_BUFFER_LIST){
    list = gst_pad_probe_info_get_buffer_list (ProbeInfo);
    if (!list){
      XGLOG_ERROR ("Buffer list is empty ");
      goto return_from_func;
    }
  }else {
    XGLOG_ERROR ("unknown Probe type ");
    goto return_from_func;
  }

  if (!src_client){
    XGLOG_ERROR ("could not get src_client pointer");
    goto free_buffer;
  }

  if (TRUE == src_client->m_StopSendRecvData){
    XGLOG_TRACE ("client is stopping freeing buffer and returning");
    goto free_buffer;
  }
  frameBin = src_client->m_FrameChannel->m_pHmpFrameBin;
  param = (list != NULL) ? (void* )list : (void*)buffer;

  XGLOG_TRACE ("handle_audioRTPPadProbeCallback() Received data from client: %s joiner_type: %d ",
                     src_client->m_detail.m_callId.c_str(), src_client->m_detail.m_joinerType);
  // check if there is activity in audio. Decode only if the voice level
  // is more than threshold . This is to reduce the number of decoding cycles
  if (TRUE == frameBin->Detect_voice_activity (src_client, param))
  {
    // push the buffer to decoder thread pool to decode
    //goto free_buffer;
    if (pSFUController->m_ThreadPool_info.m_decoder_pool){
      char   *callId_str = NULL;
      GError *error = NULL;
      xGateHmpClientThreadMsg *pMsg = NULL;
      pMsg = new xGateHmpClientThreadMsg ();
      pMsg->m_msgType = XGATE_CLIENT_DECODE_AUDIO;

      pMsg->param2 = static_cast <void*> (param);
      pMsg->pClient = src_client;

      g_mutex_lock (&src_client->m_audio_decode_process_lock);

      src_client->ullNextAudioDecodedMsgNum++;
      pMsg->ullMsgNumber = src_client->ullNextAudioDecodedMsgNum;

      g_thread_pool_push (pSFUController->m_ThreadPool_info.m_decoder_pool, pMsg, &error);

      if (error != NULL){
        src_client->ullNextAudioDecodedMsgNum--;
        g_mutex_unlock (&src_client->m_audio_decode_process_lock);
        XGLOG_ERROR ("Failed to push msg to decoder thread pool");
        g_error_free (error);
      } else {
        g_mutex_unlock (&src_client->m_audio_decode_process_lock);
        goto return_from_func;
      }
    } else{
      XGLOG_ERROR ("Decoder thread pool thread is null... can't push event");
    }
  }
free_buffer:
   if (list)
     gst_buffer_list_unref (list);
   else if (buffer)
     gst_buffer_unref (buffer);
return_from_func:
  return GST_PAD_PROBE_HANDLED ;

}

GstPadProbeReturn HmpAudioEncoderBin::handle_GstAudioMixerProbeCallback(GstPad * pad,
                                                        GstPadProbeInfo * ProbeInfo,
                                                        gpointer user_data)
{
  Client        *src_client = (Client*) user_data;
  GstBuffer     *buffer = NULL;
  SFUController *pSFUController = NULL;
  HmpAudioEncoderBin  *pAudioBin = NULL;
  pSFUController = xGateSFUtil::getSFUController();

  if (!src_client){
    XGLOG_ERROR ("src_client pointer is NULL ");
    goto return_from_func;
  }

  if ((ProbeInfo->type & GST_PAD_PROBE_TYPE_BUFFER )
           == GST_PAD_PROBE_TYPE_BUFFER){
    buffer = gst_pad_probe_info_get_buffer (ProbeInfo);
    if (!buffer){
      XGLOG_ERROR ("handle_GstAudioMixerProbeCallback () Buffer is empty ");
      goto return_from_func;
    }
  }

  XGLOG_TRACE ("Entered handle_GstAudioMixerProbeCallback () client: %s",
               src_client->m_detail.m_callId.c_str());

  if (TRUE == src_client->m_StopSendRecvData){
    gst_buffer_unref (buffer);
    return GST_PAD_PROBE_HANDLED ;
  }
  if (src_client->m_FrameChannel)
    pAudioBin = src_client->m_FrameChannel->m_pHmpAudioEncoderBin;
#ifdef AUDIO_MIXER_FILE_DUMP
    if (0 == strncmp (src_client->m_detail.m_callId.c_str(), "201", 3 ))
    {
      GstMapInfo  in_map;
      gst_buffer_map (buffer, &in_map, GST_MAP_READ);
      src_client->DumpAudioData (src_client, DUMP_MIXER_OUTPUT_DATA, in_map.data, in_map.size, nullptr);
      gst_buffer_unmap (buffer, &in_map);
    }
#endif
  if (pAudioBin && pSFUController->m_ThreadPool_info.m_encoder_pool){

      GError *error = NULL;
      xGateHmpClientThreadMsg *pMsg = NULL;
      pMsg = new xGateHmpClientThreadMsg ();
      pMsg->m_msgType = XGATE_CLIENT_ENCODE_AUDIO;

      pMsg->param2 = static_cast <void*> (buffer);
      pMsg->pClient = src_client;

      // Take reference of buffer and unref after encoding to avoid race
      // condition of buffer getting unreffed before the buffer gets encoded
      gst_buffer_ref (buffer);

      g_mutex_lock (&src_client->m_audio_encode_process_lock);

      src_client->ullNextAudioEncodeMsgNum++;
      pMsg->ullMsgNumber = src_client->ullNextAudioEncodeMsgNum;


      g_thread_pool_push (pSFUController->m_ThreadPool_info.m_encoder_pool, pMsg, &error);
      if (error != NULL){
        XGLOG_ERROR ("Failed to push event to thread pool");
        src_client->ullNextAudioEncodeMsgNum--;
        g_error_free (error);
      }
      g_mutex_unlock (&src_client->m_audio_encode_process_lock);
  } else{
    XGLOG_ERROR ("Decoder thread pool thread is null... can't push event");
  }

return_from_func:
  return GST_PAD_PROBE_HANDLED;
}

#ifdef RECORD_ENABLED
GstPadProbeReturn HmpAudioEncoderBin::handle_EncodedAudioProbeCallback (GstPad * pad,
                                                        GstPadProbeInfo * ProbeInfo,
                                                        gpointer user_data)
{
  Client *src_client = (Client*) user_data;
  GstBuffer  *buffer = NULL;
  SFUController *pSFUController = NULL;
  GstRTPBuffer rtp_buffer = {NULL};
  xGateHmpConference *pxGateHmpConfObj = NULL;
  SFUClientRcvdAudioHandler *pRcvdAudioHandler = NULL;
  HmpAudioEncoderBin  *pAudioBin = NULL;
  pSFUController = xGateSFUtil::getSFUController();
  GstClockTime clocktime = GST_CLOCK_TIME_NONE;
  mlineofferDetail *pmlineDetail = NULL;
  rtp_buffer.buffer = NULL;

  if (!src_client){
    XGLOG_ERROR ("src_client pointer is NULL ");
    goto return_from_func;
  }
  if ((ProbeInfo->type & GST_PAD_PROBE_TYPE_BUFFER )
           == GST_PAD_PROBE_TYPE_BUFFER){
    buffer = gst_pad_probe_info_get_buffer (ProbeInfo);
    if (!buffer){
      XGLOG_ERROR ("Buffer is empty ");
      goto return_from_func;
    }
  }
  if (TRUE == src_client->m_StopSendRecvData){
    XGLOG_INFO ("src_client is in Stopping state. unreffing buffer and returning");
    gst_buffer_unref (buffer);
    return GST_PAD_PROBE_HANDLED ;
  }
  XGLOG_TRACE ("handle_EncodedAudioProbeCallback() client: %s threadId: %d buffer: %p dts: %lld pts: %lld",
                     src_client->m_detail.m_callId.c_str(),
                     syscall (SYS_gettid), buffer, GST_BUFFER_DTS(buffer), GST_BUFFER_PTS(buffer));
  if (pSFUController)
     pxGateHmpConfObj = pSFUController->find_conference_entry(src_client->m_detail.m_mgresourceId);
  if (!pxGateHmpConfObj){
     XGLOG_ERROR("Failed to find entry in conference");
     gst_buffer_unref (buffer);
     goto return_from_func;
  }

  if (1 == pxGateHmpConfObj->m_clientMap.size() -1 ){
     gst_buffer_unref (buffer);
     return GST_PAD_PROBE_HANDLED;
   }
  pRcvdAudioHandler = pSFUController->get_rcvd_audio_handler();
  if(!pRcvdAudioHandler) {
    XGLOG_ERROR("Pushing a audio buffer to recorder thread failed.. ");
  }else {
    pRcvdAudioHandler->push_to_recorder (pxGateHmpConfObj, src_client, (void*)buffer);
  }
  XGLOG_TRACE ("Exit from handle_EncodedAudioProbeCallback() client: %s threadId: %d buffer: %p",
                     src_client->m_detail.m_callId.c_str(),
                     syscall (SYS_gettid), buffer);
#if 0
  return_from_func:
    return GST_PAD_PROBE_PASS;
#else
 /* Pass the buffer to rtpopuspay element. It will timestamp and send the packet
  * directly . no need to calculate the time stamp it here
  */
  pAudioBin = src_client->m_FrameChannel->m_pHmpAudioEncoderBin;
  if (src_client->m_mlineOffer_vector.size() > 0)
    pmlineDetail = src_client->m_mlineOffer_vector [0];
  if (pmlineDetail && TRUE == pmlineDetail->audioOfferDetails.bSendPkt){
    gst_rtp_buffer_map (buffer, GST_MAP_READ | GST_MAP_WRITE, &rtp_buffer);
    if (pmlineDetail->audioOfferDetails.pPeerElement)
      clocktime = gst_element_get_current_running_time (pmlineDetail->audioOfferDetails.pPeerElement);
    else if (pAudioBin->m_audiomixer)
      clocktime = gst_element_get_current_running_time (pAudioBin->m_audiomixer);
    else
      clocktime = GST_CLOCK_TIME_NONE;
    pAudioBin->check_update_rtp_timestamp (&rtp_buffer, pmlineDetail, src_client, clocktime);
    gst_rtp_buffer_unmap (&rtp_buffer);

    XGLOG_TRACE ("handle_EncodedAudioProbeCallback () client: %s clocktime: %lld" ,
                 src_client->m_detail.m_callId.c_str(), clocktime);
    if (pmlineDetail->audioOfferDetails.sinkPad){
      gst_pad_chain_without_lock (pmlineDetail->audioOfferDetails.sinkPad, buffer);
    }
  }
return_from_func:
    return GST_PAD_PROBE_HANDLED;
#endif
}

#endif

gboolean HmpFrameBin::modify_rtp_header_extn_data(GstBuffer *buffer,guint8 src_ext_id,guint8 dst_ext_id,guint8 *rtp_stream_id,guint8 *dst_rtp_stream_id,guint8 src_length,guint8 dst_length,Client *client)
{
  GstRTPBuffer rtp = { NULL };
  gst_rtp_buffer_map (buffer,GST_MAP_READ | GST_MAP_WRITE, &rtp);
  guint16 bits;
  guint8 *pdata;
  guint wordlen;
  gulong offset = 0;
  guint len = 0;
  guint8 src_rtp_stream_extn_id = src_ext_id;
  if(gst_rtp_buffer_get_extension_data(&rtp,&bits,(gpointer)&pdata,&wordlen)){

    if (bits != 0xBEDE){
      XGLOG_INFO("Unsupported Bit pattern found with callId:%s",client->m_detail.m_callId.c_str());
      return FALSE;
    }
    len = 4*wordlen;
    for (;;) {
      guint8 read_id, read_len;

      if (offset + 1 >= len)
        break;

      read_id = GST_READ_UINT8 (pdata + offset) >> 4;
      read_len = (GST_READ_UINT8 (pdata + offset) & 0x0F) + 1;
      offset += 1;

      /* ID 0 means its padding, skip */
      if (read_id == 0)
        continue;

      /* ID 15 is special and means we should stop parsing */
      if (read_id == 15)
        break;

      /* Ignore extension headers where the size does not fit */
      if (offset + read_len > len)
        break;

      /* If we have the right one */
      if (src_rtp_stream_extn_id == read_id) {
        if(src_length != read_len || dst_length != read_len){
          XGLOG_ERROR("Mismatch in expected len and actual length with callID:%s",client->m_detail.m_callId.c_str());
          return FALSE;
        }
        if(src_ext_id != dst_ext_id){
          *(pdata + offset - 1) = (dst_ext_id & 0xf) << 4 | (read_len - 1) & 0xf;
        }
        if(strcmp(rtp_stream_id,dst_rtp_stream_id)){
          memcpy(&pdata[offset],dst_rtp_stream_id,read_len);
        }
        return TRUE;
      }
      offset += read_len;

      if (offset >= len)
        break;
    }
  }
  else
  {
    //XGLOG_INFO("Did not find any header extn for callId:%s",client->m_detail.m_callId.c_str());
  }
  return FALSE;
}

gboolean HmpFrameBin::read_rtp_header_extn_data(GstBuffer *buffer,int *size,guint8 *data,guint8 extId,Client *client)
{
  GstRTPBuffer rtp = { NULL };
  gst_rtp_buffer_map (buffer,GST_MAP_READ, &rtp);
  guint16 bits;
  guint8 *pdata;
  guint wordlen;
  gulong offset = 0;
  guint len = 0;
  guint8 src_rtp_stream_extn_id = extId;
  if(gst_rtp_buffer_get_extension_data(&rtp,&bits,(gpointer)&pdata,&wordlen)){

    if (bits != 0xBEDE){
       XGLOG_INFO("Unsupported Bit pattern found for callId:%s",client->m_detail.m_callId.c_str());
       return FALSE;
    }
    len = 4*wordlen;
    for (;;) {
      guint8 read_id, read_len;

      if (offset + 1 >= len)
        break;

      read_id = GST_READ_UINT8 (pdata + offset) >> 4;
      read_len = (GST_READ_UINT8 (pdata + offset) & 0x0F) + 1;
      offset += 1;

      /* ID 0 means its padding, skip */
      if (read_id == 0)
        continue;

      /* ID 15 is special and means we should stop parsing */
      if (read_id == 15)
        break;

      /* Ignore extension headers where the size does not fit */
      if (offset + read_len > len)
        break;

      /* If we have the right one */
      if (src_rtp_stream_extn_id == read_id) {
          if(read_len > 16){
           XGLOG_INFO("Got invalid read_len%d for one byte extension header for callId:%s",read_len,
                                   client->m_detail.m_callId.c_str());
           return FALSE;
          }
          if (data){
            memcpy(data,&pdata[offset],read_len);
            data[read_len] = '\0';
          }
          if (size)
            *size = read_len;

          return TRUE;
      }
      offset += read_len;

      if (offset >= len)
        break;

      }
  }
  else
  {
    //XGLOG_INFO("Did not find any header extn for callId:%s",client->m_detail.m_callId.c_str());
  }
  gst_rtp_buffer_unmap (&rtp);
  return FALSE;
}

gboolean HmpFrameBin::fill_fixed_header(GstRTPBuffer *rtp,GstMemory **header_mem)
{
  GstMemory *mem = NULL;
  mem = gst_memory_copy(rtp->map[0].memory,0,rtp->size[0]);
  *header_mem  = mem;
  return TRUE;
}

void HmpFrameBin::fill_payload(GstRTPBuffer *rtp,GstMemory **payload_mem)
{
 GstMemory *mem = NULL;
 GstMapInfo map;
 //rtp_map does not map the payload so do it now
 gst_rtp_buffer_get_payload (rtp);
 guint payload_len = rtp->size[2];
 mem = gst_allocator_alloc(NULL, payload_len, NULL);
 gst_memory_map(mem, &map, GST_MAP_WRITE);
 if (rtp->size[2])
    memcpy(map.data,rtp->data[2],rtp->size[2]);
  gst_memory_unmap (mem, &map);
 *payload_mem  = mem;
}

GstBuffer* HmpFrameBin::check_modify_rtp_hdr_extn(Client* src_client, Client* dst_client,
		                    GstBuffer *buffer, gchar *element_name,
				    HmpFrameBin *src_frameRecvBin,
				    HmpFrameBin *dst_frameRecvBin)
{
  GstBuffer *app_buffer = NULL;
  int src_length = 0;
  int dst_length = 0;
  guint8 src_ext_id = src_frameRecvBin->m_rtp_stream_ext_id;
  guint8 dst_ext_id;
  guint8 rtp_stream_id[256];
  guint8 hdr_ext_found = 0;
  guint8 *dst_rtp_stream_id = rtp_stream_id;
  GstMemory *header_mem = NULL;
  GstMemory *payload_mem  = NULL;

  hdr_ext_found = TRUE;
  if(!src_frameRecvBin->read_rtp_header_extn_data(buffer,&src_length,
                                                 rtp_stream_id,src_ext_id,src_client)){
      //XGLOG_INFO( "Invalid header extn found in rtp packet for callId:%s",
      //                             src_client->m_detail.m_callId.c_str());
      hdr_ext_found = FALSE;
      app_buffer = buffer;
  }

  if(TRUE == hdr_ext_found){
    dst_length = strlen(dst_frameRecvBin->m_mid_video.c_str());
    dst_ext_id = dst_frameRecvBin->m_rtp_stream_ext_id;
    dst_rtp_stream_id = dst_frameRecvBin->m_mid_video.c_str();
    //Check if we just need to modify
    if((src_length == dst_length) && ((src_ext_id != dst_ext_id)
                     || strcmp(rtp_stream_id,dst_rtp_stream_id))) {
      src_frameRecvBin->modify_rtp_header_extn_data(buffer,src_ext_id,dst_ext_id,
      rtp_stream_id, dst_rtp_stream_id,
      src_length, dst_length,dst_client);
      app_buffer = buffer;
    }
    if(src_length != dst_length){
      src_frameRecvBin->write_rtp_header_data(&header_mem,&payload_mem,
                                                   buffer,dst_frameRecvBin->ssrc_video);
      src_frameRecvBin->write_rtp_header_extn_data(&app_buffer,buffer,src_client,
                                                  header_mem,payload_mem,src_ext_id,
                                                  dst_ext_id, rtp_stream_id,
                                                  dst_rtp_stream_id,src_length,dst_length);
      gst_buffer_unref(buffer);
    }
  }
  return app_buffer;
}

gboolean HmpFrameBin::write_rtp_header_data(GstMemory **header_mem,
                                            GstMemory **payload_mem,
                                            GstBuffer *buffer,guint32 ssrc)
{
  if(*header_mem != NULL || *payload_mem != NULL){
    return TRUE;
  }
  GstBuffer *rtp_pkt = NULL;
  GstRTPBuffer rtp = { NULL };
  guint32 orignal_ssrc = 0;
  gst_rtp_buffer_map (buffer,GST_MAP_READ | GST_MAP_WRITE, &rtp);
  orignal_ssrc = gst_rtp_buffer_get_ssrc(&rtp);
  gst_rtp_buffer_set_ssrc(&rtp,ssrc);
  //get the memory required for holding rtp header + payload
  fill_fixed_header(&rtp,header_mem);
  fill_payload(&rtp,payload_mem);
  gst_rtp_buffer_set_ssrc(&rtp,orignal_ssrc);
  gst_rtp_buffer_unmap(&rtp);
  //Copy the meta info from src buffer
  return TRUE;
}

GstMemory* HmpFrameBin::copy_ext_mem(guint8* ext_mem,guint num_bytes)
{
  GstMemory *mem = gst_allocator_alloc(NULL,num_bytes,NULL);
  GstMapInfo map;
  gst_memory_map(mem, &map, GST_MAP_WRITE);
  memcpy(map.data,ext_mem,num_bytes);
  gst_memory_unmap(mem,&map);
  return mem;
}

GstBuffer* HmpFrameBin::make_new_buffer(GstBuffer *src,GstMemory *hdr_mem,GstMemory *ext_mem,
		                            GstMemory *payload_mem)
{
  GstBuffer *buffer = gst_buffer_new();
  gst_buffer_append_memory(buffer,hdr_mem);
  gst_buffer_append_memory(buffer,ext_mem);
  gst_buffer_append_memory(buffer,payload_mem);
  if(!gst_buffer_copy_into(buffer,src,GST_BUFFER_COPY_METADATA,0,-1)){
    XGLOG_INFO("Could not copy metadata into dst buffer");
    gst_buffer_unref(buffer);
    return NULL;
  }
  return buffer;
}

gboolean HmpFrameBin::write_rtp_header_extn_data(GstBuffer **dst,GstBuffer *src,
                                                 Client *client,GstMemory *hdr_mem,
		                                 GstMemory *payload_mem,
		                                 guint8 src_ext_id,
		                                 guint8 dst_ext_id,guint8 *src_rtp_id,
                                                 guint8 *dst_rtp_id,
						 guint8 src_length,guint8 dst_length)
{
  GstRTPBuffer rtp = { NULL };
  GstMemory *ext_mem = NULL;
  guint16 bits;
  guint8 *pdata;
  guint wordlen;
  gulong offset = 0;
  guint len = 0;
  if(!gst_rtp_buffer_map(src, GST_MAP_READ | GST_MAP_WRITE, &rtp)){
    XGLOG_INFO("Could not map GstBuffer to RTP Buffer for callId:%s",
                                   client->m_detail.m_callId.c_str());
    return FALSE;
  }
  if(gst_rtp_buffer_get_extension_data(&rtp,&bits,(gpointer)&pdata,&wordlen)){
    guint8 mod_ext[wordlen * 4 + 100 + 4];
    guint mod_offset = 4;
    if (bits != 0xBEDE){
      XGLOG_INFO("Unsupported Bit pattern found");
      return FALSE;
    }
    len = 4*wordlen;
    for (;;) {
      guint8 read_id, read_len;

      if (offset + 1 >= len)
        break;

      read_id = GST_READ_UINT8 (pdata + offset) >> 4;
      read_len = (GST_READ_UINT8 (pdata + offset) & 0x0F) + 1;
      offset += 1;

      /* ID 0 means its padding, skip */
      if (read_id == 0)
        continue;

      /* ID 15 is special and means we should stop parsing */
      if (read_id == 15)
        break;

      /* Ignore extension headers where the size does not fit */
      if (offset + read_len > len)
        break;


      if(src_ext_id == read_id) {
        mod_ext[mod_offset] = (dst_ext_id & 0xf) << 4 | (((dst_length - 1) & 0xf));
        mod_offset++;
        memcpy(&mod_ext[mod_offset],dst_rtp_id,dst_length);
        mod_offset += dst_length;
      }
      else
      {
        mod_ext[mod_offset] = (read_id & 0xf) << 4 | (read_len - 1) & 0xf;
        mod_offset++;
        memcpy(&mod_ext[mod_offset],&pdata[offset],read_len);
        mod_offset += read_len;
      }

    }
    //Now fill the header
    mod_ext[0] = 0xBE;
    mod_ext[1] = 0xDE;
    //guint num_words = mod_offset/4 + ((mod_offset % 4) > 0) ? 1 : 0 ;
    guint8 num_bytes_pad = ((mod_offset % 4) > 0) ? (4 - (mod_offset % 4)) : 0;
    if(num_bytes_pad){
        memset(&mod_ext[mod_offset],0,num_bytes_pad);
    }
    mod_offset += num_bytes_pad;
    //Store the number of hdr ext words
    mod_ext[2] = ((mod_offset - 4)/4) & 0xff00;
    mod_ext[3] = ((mod_offset - 4)/4) & 0xff;
    ext_mem = copy_ext_mem(mod_ext,mod_offset);
    *dst = make_new_buffer(src,hdr_mem,ext_mem,payload_mem);
  }
  else
  {
    XGLOG_ERROR("Could not find any header extn data for callId:%s",
                                  client->m_detail.m_callId.c_str());
    return FALSE;
  }
  gst_rtp_buffer_unmap(&rtp);
  return TRUE;
}

void HmpFrameBin::on_new_data_rcvd_srcPad (Client *src_client,
                                          gchar *element_name,
                                          void * vBuffObj)
{
  HmpFrameBin   *frameBin = NULL;
  SFUController *pSFUController = NULL;
  xGateHmpConference *pxGateHmpConfObj = NULL;
  Client          *dst_client = NULL;
  GstBufferList   *list = NULL;
  GstBuffer       *buffer = NULL;
  guint           num_buffers = 0;
  bool            bPostData  = FALSE;
  StreamType      eStreamType = STREAM_TYPE_UNKNOWN;
  SFUClientRcvdAudioHandler *pRcvdAudioHandler = NULL;
  void            *pvParam = NULL;
  pSFUController = xGateSFUtil::getSFUController();

  if (!src_client){
    XGLOG_ERROR ("src_client pointer is NULL ");
    goto return_from_func;
  }
  if (pSFUController)
     pxGateHmpConfObj = pSFUController->find_conference_entry(src_client->m_detail.m_mgresourceId);

  if (!pxGateHmpConfObj){
     XGLOG_ERROR("Failed to find entry in conference");
     goto return_from_func;
  }

  if (GST_IS_BUFFER_LIST (vBuffObj))
    list = GST_BUFFER_LIST_CAST (vBuffObj);
  else
    buffer = GST_BUFFER_CAST (vBuffObj);

  if (list){
    num_buffers = gst_buffer_list_length (list);
  }else if (buffer){
    num_buffers = 1;
  }else {
    XGLOG_ERROR ("Received Neither list: %p nor buffer: %p", list, buffer);
    goto return_from_func;
  }

  XGLOG_TRACE ("on_new_data_rcvd_srcPad () Received data from client: %s element_name: %s joiner_type: %d ",
                     src_client->m_detail.m_callId.c_str(), element_name, src_client->m_detail.m_joinerType);
  if (strstr (element_name, "Audio")){
     eStreamType = STREAM_TYPE_AUDIO;
  }
  else if (strstr (element_name, "Video"))
     eStreamType = STREAM_TYPE_VIDEO;
  else if (strstr (element_name, "screenshare")){
     eStreamType = STREAM_TYPE_SCREENSHARE;
  }else {
     XGLOG_INFO ("on_new_data_rcvd_srcPad () unkonwn streamType from client: %s element_name: %s joiner_type: %d ",
                     src_client->m_detail.m_callId.c_str(),
                     element_name, src_client->m_detail.m_joinerType);
    goto return_from_func;
  }
  frameBin = src_client->m_FrameChannel->m_pHmpFrameBin;

  if (eStreamType == STREAM_TYPE_AUDIO){
    frameBin->m_ulNumAudioPktRcvd += num_buffers;
    if(0 == (frameBin->m_ulNumAudioPktRcvd % LOG_DEBUG_PRINT_NUM_PACKET)){
      XGLOG_INFO ("client: %s AUDIO pad_name: %s num_audio_pkts_rcvd: %lu",
                  src_client->m_detail.m_callId.c_str(),
                  element_name, frameBin->m_ulNumAudioPktRcvd);
     }
     /* Send one copy of audio buffer to STT client */
     if (src_client->m_detail.m_joinerType != EN_JOINER_TYPE_SCREENSHARE)
       pRcvdAudioHandler->push_to_mrcp (src_client, vBuffObj);

    #ifdef RECORD_ENABLED
    #ifndef AUDIO_MCU_ENABLED
     pRcvdAudioHandler = pSFUController->get_rcvd_audio_handler();
     if(!pRcvdAudioHandler) {
       XGLOG_ERROR("Pushing a audio buffer to recorder thread failed.. ");
     }else {
       XGLOG_INFO ("client: %s calling push_to_recorder", src_client->m_detail.m_callId.c_str());
       pRcvdAudioHandler->push_to_recorder (pxGateHmpConfObj, src_client, vBuffObj);
     }
    #endif
    #endif
   }
   else if ((eStreamType == STREAM_TYPE_VIDEO) ||  (eStreamType == STREAM_TYPE_SCREENSHARE)){
     frameBin->m_ulNumVideoPktRcvd += num_buffers;
     if (0 == (frameBin->m_ulNumVideoPktRcvd % LOG_DEBUG_PRINT_NUM_PACKET))
       XGLOG_INFO ("client: %s VIDEO  pad_name: %s num_video_pkts_rcvd: %lu",
                   src_client->m_detail.m_callId.c_str(),
                  element_name, frameBin->m_ulNumVideoPktRcvd);
    #ifdef RECORD_ENABLED
     SFUClientRcvdVideoHandler *pRcvdVideoHandler = pSFUController->get_rcvd_video_handler();
     if(!pRcvdVideoHandler) {
       XGLOG_ERROR("Pushing a video buffer to recorder thread failed.. ");
     }else {
       pRcvdVideoHandler->push_to_recorder (pxGateHmpConfObj, src_client, list);
     }
    #endif
   }
    /* New client needs to be extracted */
   for (auto itr = pxGateHmpConfObj->m_clientMap.begin();
                  itr != pxGateHmpConfObj->m_clientMap.end(); itr++){
     dst_client = itr->second;
     if ((NULL == dst_client)
         || ((dst_client == src_client) && (eStreamType == STREAM_TYPE_AUDIO))){
        continue;
      } else if ((NULL == dst_client) ||
         (((dst_client == src_client) && (eStreamType == STREAM_TYPE_VIDEO)
                     && (dst_client->m_mid3CallId.empty())))){
        continue;
     }else if ((TRUE != dst_client->m_StopSendRecvData)
                && (dst_client->m_mlineOffer_map.size () > 0)){
       /*Get the mline offer details and check if sink pad
        *is created and we post data
        */
       mlineofferDetail  *mlineDetails = NULL;
       MLINE_OFFER_MAP::iterator it;
       mlineofferDetail  *mlineDetails_mid3 = nullptr;
       if ((dst_client->m_viewType == XGATE_VIEW_TYPE_ACTIVE_SPEAKER
               && (eStreamType == STREAM_TYPE_SCREENSHARE))
           || (!dst_client->m_mid3CallId.empty()
              //&& dst_client->m_mid3CallType == XGATE_MID3_CALL_TYPE_ACTIVE_SPEAKER
              && dst_client->m_viewType == XGATE_VIEW_TYPE_ACTIVE_SPEAKER
              && eStreamType == STREAM_TYPE_VIDEO)){
         it = dst_client->m_mlineOffer_map.find ("screenshare");
         if (it != dst_client->m_mlineOffer_map.end ())
           mlineDetails_mid3 = (mlineofferDetail*)it->second;
           XGLOG_TRACE ("mlineDetails_mid3: %p element: %s stream_type: %d for dst_client: %s",
                          mlineDetails_mid3,element_name,eStreamType,
                                                dst_client->m_detail.m_callId.c_str());
       }
       if (eStreamType == STREAM_TYPE_VIDEO){
         it = dst_client->m_mlineOffer_map.find (src_client->m_detail.m_callId);

         if (it != dst_client->m_mlineOffer_map.end ()){
           mlineDetails = (mlineofferDetail*)it->second;
         }
       }
       if (eStreamType == STREAM_TYPE_AUDIO){
       /* if Audio MCU is enabled then we need to pass buffer to get it
        * mixed even though if video is not transmitted due to Tile based
        * request
        */
       #ifdef AUDIO_MCU_ENABLED
         AUDIO_MIXER_INPUT_INFO  *pMixerInput = NULL;
         bPostData = FALSE;
         HmpAudioEncoderBin *AudioEncoderBin = dst_client->m_FrameChannel->m_pHmpAudioEncoderBin;
         if (!AudioEncoderBin)
           continue;
       #ifdef USE_GST_AUDIO_MIXER_PLUGIN
         for (guint32 uiIndex = 0 ; uiIndex < AudioEncoderBin->m_mixer_input_info.size(); uiIndex++){
            pMixerInput = AudioEncoderBin->m_mixer_input_info[uiIndex];
            if (!pMixerInput)
              continue;
            XGLOG_TRACE ("dst_client: %s src_client: %s pMixerInput->src_callId: %s sinkpad: %p",
                          dst_client->m_detail.m_callId.c_str(),
                          src_client->m_detail.m_callId.c_str(),
                          pMixerInput->src_callId.c_str(),
                          pMixerInput->sink_pad );
            if ((pMixerInput->src_callId == src_client->m_detail.m_callId)
                && pMixerInput->sink_pad && (TRUE == pMixerInput->bStartEventSent)){
              XGLOG_TRACE ("Audio buffer received from client %s .. pushing to dst_client: %s",
                       pMixerInput->src_callId.c_str(), dst_client->m_detail.m_callId.c_str());
              bPostData = TRUE;
              break;
            }
         }
         if (bPostData == TRUE){
           pvParam = (void *)pMixerInput;
         }
       #else
        /* As there is only one m-line for audio using mixer implementation, the
         * first array will have offer details*/
        mlineDetails = dst_client->m_mlineOffer_vector.at (0);
        if (mlineDetails && TRUE == mlineDetails->audioOfferDetails.bSendPkt ){
           bPostData = TRUE;
           pvParam = (void *)src_client;
        }
       #endif
       #else
           if (!mlineDetails){
              mlineofferDetail  *pTmpmline = NULL;
              for (guint32 uiIndex = 0 ; uiIndex < dst_client->m_mlineOffer_vector.size(); uiIndex++){
                pTmpmline = dst_client->m_mlineOffer_vector.at (uiIndex);
                XGLOG_TRACE ("pTmpmline->callId_offer_generated: %s data rcvd from m_callId: %s TO send callId: %s",
                    pTmpmline->callId_offer_generated.c_str(),  src_client->m_detail.m_callId.c_str(),
                    dst_client->m_detail.m_callId.c_str());
                if ((0 == strcmp (pTmpmline->callId_offer_generated.c_str(), "audio_only")) ||
                    (0 == strcmp (pTmpmline->callId_offer_generated.c_str(), "screenshare")) ||
                    (strstr (pTmpmline->callId_offer_generated.c_str(), EMPTY_DUMMY_CALLID))){
                  continue;
                }
                mlineDetails = pTmpmline;
                if (pTmpmline->callId_offer_generated == src_client->m_detail.m_callId){
                   break;
                 }
               }
            }
           if (mlineDetails && TRUE == mlineDetails->audioOfferDetails.bSendPkt ){
              bPostData = TRUE;
              pvParam = (void*)mlineDetails;
           }
       #endif
       }else if (eStreamType == STREAM_TYPE_VIDEO){
           if (mlineDetails && (TRUE == mlineDetails->videoOfferDetails.bSendPkt )){
              bPostData = TRUE;
              pvParam = (void*)mlineDetails;
            }
           // check if mid3 line is in active speaker or screen share mode.
           //if (dst_client->m_mid3CallType == XGATE_MID3_CALL_TYPE_ACTIVE_SPEAKER){
           if (dst_client->m_viewType == XGATE_VIEW_TYPE_ACTIVE_SPEAKER){

                 XGLOG_TRACE ("src_client: %s dst_client: %s callId_offer_generated: %s m_mid3CallId: %s ",
                 src_client->m_detail.m_callId.c_str(), dst_client->m_detail.m_callId.c_str(),
                 (mlineDetails == nullptr) ? "" : mlineDetails->callId_offer_generated.c_str(),
                 dst_client->m_mid3CallId.c_str());
                 /*
                  * Send Data received to the destination client via active speaker mid line if
                  * 1. dst client is in tile map and source client callId matches with mid3 calId
                  * 2. dst client mid3 is equal to it's callId i,,e dst client is active speaker
                  * 3. dst client is not in tile map and dst mid3 callId is not empty
                  */
              if ((mlineDetails && mlineDetails->callId_offer_generated == dst_client->m_mid3CallId)
                  || ((dst_client->m_mid3CallId == dst_client->m_detail.m_callId)
                      && (src_client == dst_client))
                  || (!mlineDetails && dst_client->m_mid3CallId == src_client->m_detail.m_callId)) {

                   XGLOG_TRACE ("sending Packets on mid3 for dst_client: %s mid3callId: %s mlineDetails_mid3: %p",
                           dst_client->m_detail.m_callId.c_str(), dst_client->m_mid3CallId.c_str(),
                           mlineDetails_mid3 );
                 if (mlineDetails_mid3 && TRUE == mlineDetails_mid3->videoOfferDetails.bSendPkt){
                   PostBufferToThread (dst_client, element_name,
                              (void*)list, TRUE, (void*)mlineDetails_mid3);
                       // TODO: need to avoid sending the same data 2 times, one in
                       // mid3 line and other time through the negotiated offer line
                       // Should continue from here. should be done after discussing with
                       // UI team so that they remove showing the grids along the active
                       // speaker for the same paticipant.
                    //continue;
                  }
              }
           }
       } else if (mlineDetails_mid3 && (eStreamType == STREAM_TYPE_SCREENSHARE)){
           mlineDetails = mlineDetails_mid3;
           XGLOG_TRACE ("Packets received from screen share element from src_client: %s sending to dst_client: %s",
                         src_client->m_detail.m_screenshare_id.c_str(),
                         dst_client->m_detail.m_callId.c_str());
           if (!src_client->m_detail.m_screenshare_id.empty()
                  && (0 != strcmp (dst_client->m_detail.m_callId.c_str(),
                    src_client->m_detail.m_screenshare_id.c_str()))) {
             XGLOG_TRACE ("screen share pkts rcvd posting to dst_client: %s mlineDetails: %p bPostData: %d",
                            dst_client->m_detail.m_callId.c_str(), mlineDetails,
                 mlineDetails->videoOfferDetails.bSendPkt );
             if (TRUE == mlineDetails->videoOfferDetails.bSendPkt ){
               bPostData = TRUE;
               pvParam = (void*)mlineDetails;
             }
           }
       }
       if (TRUE == bPostData){
         if (list)
           PostBufferToThread (dst_client, element_name,
                              (void*)list, TRUE, pvParam);
         else if (buffer)
           PostBufferToThread (dst_client, element_name, buffer, FALSE, pvParam);
       }
     }
     bPostData = FALSE;
     dst_client = NULL;
   }
 return_from_func:
   /* unref the list */
   if (list)
     gst_buffer_list_unref (list);
   else if (buffer)
     gst_buffer_unref (buffer);
}

xGateRetVal HmpFrameBin::PostBufferToThread (Client *dst_client,
                                              gchar *element_name,
                                              void *buffer,
                                              gboolean bIsList,
                                              void *mlineDetails)
{
  ACE_Message_Block* pAmb = NULL;
  xGateHmpClientThreadMsg *pMsg = NULL;
  ACE_Time_Value tval ((time(NULL) ));
  xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;

  if (!dst_client->send_data_msg_q) {
     return EN_XGATE_STATUS_SUCCESS;
  }
  pMsg = new xGateHmpClientThreadMsg ();
  tval += ACE_Time_Value(0,1);
  strcpy (pMsg->ucName, element_name);
  pMsg->m_msgType = XGATE_CLIENT_SEND_RTP_DATA;
  if (bIsList){
    GstBufferList *buffer_list = (GstBufferList *) buffer;
    GstBufferList *app_buffer_list = gst_buffer_list_copy_deep (buffer_list);
    pMsg->param2 = static_cast <void*> (app_buffer_list);
  }else {
    GstBuffer *buf = (GstBuffer *) buffer;
    GstBuffer *app_buffer = gst_buffer_copy_deep (buf);
    pMsg->param2 = static_cast <void*> (app_buffer);
  }
  pMsg->param3 = static_cast <void*> (mlineDetails);
  pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
#ifdef LOG_RTP_SEQNUM_EVERY_VIDEO_PKT
  if(strstr(element_name, "Video")){
    GstRTPBuffer rtp_buffer = {NULL};
    rtp_buffer.buffer = NULL;
    gst_rtp_buffer_map (app_buffer, GST_MAP_READ | GST_MAP_WRITE, &rtp_buffer);
    int payload_type = gst_rtp_buffer_get_payload_type (&rtp_buffer);
    int current_seq_num = gst_rtp_buffer_get_seq (&rtp_buffer);
    int current_pkt_time = gst_rtp_buffer_get_timestamp (&rtp_buffer);
    XGLOG_INFO ("PostBufferToThread() element: %s rcvd_seq_num: %hu
          current_pkt_time: %u payload: %d",
           element_name, current_seq_num, current_pkt_time,payload_type);
    gst_rtp_buffer_unmap (&rtp_buffer);
  }
#endif

#ifndef USE_SEPARATE_THREAD_TO_SEND_DATA
  pMsg->m_msgType = XGATE_CLIENT_RCVD_RTP_DATA;
  if(!dst_client->putq(pAmb, &tval)){
    XGLOG_ERROR("Failed to push RTP data  to Client thread");
    retVal = EN_XGATE_STATUS_ERROR;
  }
#else
  if(strstr(element_name, "Audio")){
#ifdef AUDIO_MIXER_FILE_DUMP
    GstMapInfo  in_map;
    GstBuffer *app_buffer = NULL;
    SFUController *pSFUController = NULL;
    Client *src_client = NULL;
    pSFUController = xGateSFUtil::getSFUController();
    xGateHmpConference   *pHmpConfObj = nullptr;
    pHmpConfObj = pSFUController->find_conference_entry(dst_client->m_detail.m_mgresourceId);
    if (!pHmpConfObj){
      XGLOG_ERROR ("Failed to get pHmpConfObj for mgresourceId: %s client: %s",
                   dst_client->m_detail.m_mgresourceId.c_str(), dst_client->m_detail.m_callId.c_str());
    }
    #ifdef USE_GST_AUDIO_MIXER_PLUGIN
    mlineofferDetail *pmline = (mlineofferDetail *)mlineDetails;
    AUDIO_MIXER_INPUT_INFO  *pMixerInput = NULL;
    pMixerInput = (AUDIO_MIXER_INPUT_INFO *)mlineDetails;
    src_client = pHmpConfObj->get_participant_entry (pMixerInput->src_callId);
    if (!src_client){
      XGLOG_ERROR ("Failed to get client Pointer for callId: %s ",
                        pMixerInput->src_callId.c_str());
      return FALSE;
    }
    #else
      src_client = (Client *) mlineDetails;
    #endif // USE_GST_AUDIO_MIXER_PLUGIN
      app_buffer = (GstBuffer*) pMsg->param2;

      gst_buffer_map (app_buffer, &in_map, GST_MAP_READ);
      dst_client->DumpAudioData (dst_client, DUMP_DECDODER_DATA, in_map.data, in_map.size, src_client);
      gst_buffer_unmap (app_buffer, &in_map);
#endif // AUDIO_MIXER_FILE_DUMP


  #ifdef USE_GST_AUDIO_MIXER_PLUGIN
    if(dst_client->send_data_msg_q && !dst_client->send_data_msg_q->enqueue_prio(pAmb, &tval)){
      XGLOG_ERROR("Failed to push audio to mixer plugin ");
      retVal = EN_XGATE_STATUS_ERROR;
    }
  #else
    pMsg->m_msgType = XGATE_CLIENT_MIX_AUDIO;
    if(dst_client->mixer_msg_q && !dst_client->mixer_msg_q->enqueue_prio(pAmb, &tval)){
      XGLOG_ERROR("Failed to push  data to Client mixer Q thread");
      retVal = EN_XGATE_STATUS_ERROR;
    }
  #endif
  }else if ((strstr(element_name, "Video") || strstr(element_name, "screenshare"))
            && dst_client->send_data_msg_q) {
    if(!dst_client->send_data_msg_q->enqueue_prio(pAmb, &tval)){
      XGLOG_ERROR("Failed to push RTP data to Client thread");
      retVal = EN_XGATE_STATUS_ERROR;
    }
  }
  #endif
  return retVal;
}

void HmpFrameBin::handle_data_rcvd_rx_thread (Client *client, gchar *element_name,
                                            GstBuffer *buffer,
                                            void *pvParam)
{
  HmpFrameBin *frameRecvBin = this;
  mlineofferDetail *mlineDetails = NULL;
  /*
   * 1. If it's conference ,we need to send data to all clients except the one
   * from which data is received
   *
   * 2. If it's one-one call, extract the other client pointer and send it
   * */
  if(EN_XGATE_CALL_TYPE_SIMPLE_VIDEO == client->m_detail.m_callType){
     SendRTPPktToDstClient (client,element_name, buffer, frameRecvBin, pvParam);
  }
  else if (EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO == client->m_detail.m_callType){
    if (strstr(element_name, "Audio")){
      #ifdef AUDIO_MCU_ENABLED
        SendRTPPktToDstClient (client, element_name, buffer,
                              frameRecvBin, pvParam);
      #else
        mlineDetails = (mlineofferDetail*)pvParam;
        if (TRUE == mlineDetails->audioOfferDetails.bSendPkt){
           SendRTPPktToDstClient (client, element_name, buffer,
                              frameRecvBin,  pvParam);
        }
      #endif
    }
    else if (strstr(element_name, "Video") || strstr(element_name, "screenshare")){
      mlineDetails = (mlineofferDetail*)pvParam;
      if (TRUE == mlineDetails->videoOfferDetails.bSendPkt){
        SendRTPPktToDstClient (client, element_name, buffer,
                              frameRecvBin, pvParam);
      }
    }else{
      XGLOG_ERROR ("Received pkts from unknown element : %s", element_name );
      if (GST_IS_BUFFER_LIST (buffer))
         gst_buffer_list_unref (GST_BUFFER_LIST_CAST(buffer));
      else
         gst_buffer_unref (buffer);
    }
  }
}

void HmpFrameBin::SendRTPPktToDstClient (Client *client,
		                         gchar *element_name,
		                         GstBuffer *buffer,
					 HmpFrameBin *frameRecvBin,
                                         void *pvParam)
{
  offerDetail *pOfferDetail = NULL;
  GstBufferList *app_buffer_list = NULL;
  GstBuffer *app_buffer = NULL;
  guint     num_buffers = 0;
  gchar     *dst_element_name = NULL;
  gboolean  bIsBufferFromList = TRUE;
  GstBufferList *pBufferList = NULL;
  GstClockTime  clocktime = GST_CLOCK_TIME_NONE;
  AUDIO_MIXER_INPUT_INFO *pAudioMixInp = NULL;
  mlineofferDetail    *mlineDetails = NULL;
  GstPad              *sinkpad = NULL;
  gboolean             bSendPkt = FALSE;
  GstElement           *pPeerElement = NULL;
  GstElement           *appsrc = NULL;
  //XGLOG_INFO("Entered SendRTPPktToDstClient name: %s webrtcbin: %p",
  //                 element_name, dst_frameRecvBin->m_webrtcbin);
  /* TODO:
   * 1. check if data is from client that support's simulcast
   * extract the rid from rtp extension header.
   *
   * 2. Logic to decide which stream need to be sent other client and
   * criteria to be decided
   *
   * 3. For time being, configuring data_from_rid based on header extension
  */
  if (strstr(element_name, "Audio")){
   #ifdef AUDIO_MCU_ENABLED
    pAudioMixInp = (AUDIO_MIXER_INPUT_INFO*)pvParam;
    sinkpad = pAudioMixInp->sink_pad;
    bSendPkt = TRUE;
    pPeerElement = pAudioMixInp->pPeerElement;
    appsrc = pAudioMixInp->m_appsrc;
   #else
    mlineDetails = (mlineofferDetail *)pvParam;
    pOfferDetail = &mlineDetails->audioOfferDetails;
    sinkpad = pOfferDetail->sinkPad;
    bSendPkt = pOfferDetail->bSendPkt;
    pPeerElement = pOfferDetail->pPeerElement;
    appsrc = pOfferDetail->m_appsrc;
   #endif
  } else {
    mlineDetails = (mlineofferDetail *)pvParam;
    pOfferDetail = &mlineDetails->videoOfferDetails;
    sinkpad = pOfferDetail->sinkPad;
    bSendPkt = pOfferDetail->bSendPkt;
    pPeerElement = pOfferDetail->pPeerElement;
    appsrc = pOfferDetail->m_appsrc;
  }
  if(((GstPad *)0 == sinkpad)
      || (FALSE == bSendPkt)) {
    XGLOG_ERROR ("sinkPad not created for .. can't Tx data");
    if (GST_IS_BUFFER_LIST (buffer))
       gst_buffer_list_unref (GST_BUFFER_LIST_CAST(buffer));
    else
       gst_buffer_unref (buffer);
    return ;
  }
  if (GST_IS_BUFFER_LIST (buffer)){
     pBufferList = GST_BUFFER_LIST_CAST (buffer);
     num_buffers = gst_buffer_list_length (pBufferList);
  } else {
    bIsBufferFromList = FALSE;
    num_buffers = 1;
  }

  clocktime = gst_element_get_current_running_time (pPeerElement);

  /*
   * if VP9 is negotiated and SVC is enabled, parse VP9 payload and
   * based on layer selected, remove the other layers and make list
   * of packets that need to be sent
   */
  if ((strstr(element_name, "Video") || strstr(element_name, "screenshare"))
           && strstr(pOfferDetail->encoding_name, "VP9")){
    GstBufferList  *pVP9BuffList = NULL;
    VideoRtpDepacketizerVp9 vp9parseobj;
    // lock the object before processing the buffer list and
    // unlock it after processing it
    pVP9BuffList = vp9parseobj.ProcessVP9Payload (client,
                                  mlineDetails, pBufferList, clocktime);
    if (pBufferList){
      gst_buffer_list_unref (pBufferList);
    }

    if (!pVP9BuffList){
      return;
    }
    if(mlineDetails->videoOfferDetails.bTileChanged){
      XGLOG_INFO ("Tile change request Key Frame Sending...");
      send_key_frame_request_to_mbc (client, mlineDetails->callId_offer_generated);
      mlineDetails->videoOfferDetails.bTileChanged = FALSE;
      return;
    }
    pBufferList = pVP9BuffList;
    goto push_buffer_sink_pad;
  }

  /*
   * set the ssrc to what was negotiated
   */
  for (guint uiIndex = 0; uiIndex < num_buffers; uiIndex++) {
    if (pBufferList) {
      app_buffer = gst_buffer_list_get (pBufferList, uiIndex);
    }else {
      app_buffer = buffer;
    }
    if (clocktime != GST_CLOCK_TIME_NONE){
      GST_BUFFER_DTS (app_buffer) = GST_CLOCK_TIME_NONE;
      GST_BUFFER_PTS (app_buffer) = clocktime;
    }
    if (strstr(element_name, "Audio")){
#ifdef AUDIO_MCU_ENABLED
      if (appsrc) {
        gst_app_src_push_buffer (GST_APP_SRC (appsrc), app_buffer);
      }else{
        gst_pad_chain_without_lock (sinkpad, app_buffer);
      }
      return;
#endif
      if (mlineDetails->audioOfferDetails.uiTxSSRCNum){
         app_buffer_list = frameRecvBin->Modify_RTP_Header (element_name, app_buffer,
                       &mlineDetails->audioOfferDetails, bIsBufferFromList);
       }

    }else if (strstr(element_name, "Video_RTX")
               && (mlineDetails->videoOfferDetails.uiRTXTxSSRCNum)){
      app_buffer_list = frameRecvBin->Modify_RTP_Header (element_name, app_buffer,
                       &mlineDetails->videoOfferDetails, bIsBufferFromList);

    }else if (strstr(element_name, "Video") || strstr(element_name, "screenshare")
                && (mlineDetails->videoOfferDetails.uiTxSSRCNum)){
      if(mlineDetails->videoOfferDetails.bTileChanged){
        XGLOG_ERROR ("Tile change request Key Frame Sending...");
        send_key_frame_request_to_mbc (client, mlineDetails->callId_offer_generated);
        mlineDetails->videoOfferDetails.bTileChanged = FALSE;
        return;
      }
      app_buffer_list = frameRecvBin->Modify_RTP_Header (element_name, app_buffer,
                         &mlineDetails->videoOfferDetails, bIsBufferFromList);
    }
  }
  /* if input is buffer list assign the received i/p buffer list
   * other wise the buffer list will be prepared by RTP_Modify_Header
   * once a frame boundary is completed and NULL if it's still adding to
   * list
   */
push_buffer_sink_pad:
  if (pBufferList){
     app_buffer_list = pBufferList;
  }
   /*
    * create buffer list and push buffer list instead of pushing one by one
    */
  if (app_buffer_list && pOfferDetail->sinkPad){
    gst_pad_chain_list_without_lock (pOfferDetail->sinkPad, app_buffer_list);
  } else {
    gst_buffer_list_unref (app_buffer_list);
  }
}

GstBufferList *HmpFrameBin::Modify_RTP_Header (gchar *element_name,
                                     GstBuffer *buffer,
                                     offerDetail *pOfferDetails,
                                     gboolean bIsBufferFromList)
{
  HmpFrameBin *frameRecvBin = this;
  guint payload_type = 0;
  unsigned short  current_seq_num = 0;
  short seq_diff = 0;
  guint32 current_pkt_time = 0;
  guint32 u32RcvSSRC = 0;
  int timestamp_diff = 0;
  guint32 u32SSRC = 0;
  GstRTPBuffer rtp_buffer = {NULL};
  GstBufferList *pBufferList = NULL;
  gpointer audio_level = 0xFF;
  guint  size = 1;
   /* Modify the ssrc in RTP packet and send it */
  rtp_buffer.buffer = NULL;
  if (TRUE != gst_rtp_buffer_map (buffer, GST_MAP_READWRITE, &rtp_buffer)){
    XGLOG_ERROR("rtp buffer could not be mapped");
    return NULL;
  }

  payload_type = gst_rtp_buffer_get_payload_type (&rtp_buffer);
  current_seq_num = gst_rtp_buffer_get_seq (&rtp_buffer);
  current_pkt_time = gst_rtp_buffer_get_timestamp (&rtp_buffer);
  u32RcvSSRC = gst_rtp_buffer_get_ssrc (&rtp_buffer);

  if (!current_pkt_time && !payload_type && !current_seq_num){
    XGLOG_ERROR ("Invalid rtp map buffer ");
    gst_rtp_buffer_unmap (&rtp_buffer);
    return NULL;
  }
#ifdef LOG_RTP_SEQNUM_EVERY_VIDEO_PKT
  if (strstr(element_name, "Video"))
    XGLOG_INFO ("Modify_RTP_Header() element: %s current_seq_num: %u current_pkt_time: %u payload: %d",
                   element_name, current_seq_num, current_pkt_time,payload_type);
#endif
  /* if we haven't sent any packets till now, sequence diff is 0*/
  if (!pOfferDetails->ulNumPktSent){
    seq_diff = 1;
    timestamp_diff = 0;
  }else{
    seq_diff = current_seq_num + ~pOfferDetails->usPrevRcvdSeqNum + 1;
    timestamp_diff = current_pkt_time - pOfferDetails->uiPrevRTPTimestamp;
  }
  /* if sequence number diff is more than half of the unsigned short , SRTP library
   * In those case, reset the sequence number diff to 1
   */
  if (seq_diff >= MAX_RTP_SEQNUM/2)
    seq_diff = 1;

  if (seq_diff != 1){
    XGLOG_TRACE ("element: %s_rcv_ssrc_%u usPrevRcvdSeqNum: %hu \
                  current_seq_num: %hu seq_diff: %hd",
                  element_name, u32RcvSSRC, pOfferDetails->usPrevRcvdSeqNum,
                   current_seq_num, seq_diff);
    if(pOfferDetails->usSeqNum > 1){
       seq_diff = 1;
    }
  }
  #if 1
  if((timestamp_diff < 0 || timestamp_diff > 12000)){
    XGLOG_INFO ("element:  %s usPrevRcvdSeqNum: %u current_seq_num: %u seq_diff: %hd \
                   last_tx_seqnum: %hu  to SSRC:%ld currentpkttime:%d ,PrevRcvdTimestamp:%d,\
                   timestamp_diff:%d , last_ts_sent:%d , payload:%d",
             element_name,pOfferDetails->usPrevRcvdSeqNum, current_seq_num, seq_diff,
             pOfferDetails->usSeqNum,pOfferDetails->uiTxSSRCNum,current_pkt_time,
             pOfferDetails->uiPrevRTPTimestamp,timestamp_diff,pOfferDetails->uiTxTimestamp,
             payload_type);
    timestamp_diff = 0;
  }
  #endif
  if (strstr(element_name, "RTX") && (pOfferDetails->uiRTXTxSSRCNum)){
    XGLOG_INFO("calling RTX ssrc: %u payload_num: %d seq_num: %hu element_name: %s",
                      pOfferDetails->uiRTXTxSSRCNum,
                      pOfferDetails->uiRTXPayloadNum,
                      pOfferDetails->usRTXSeqNum, element_name);
    u32SSRC = pOfferDetails->uiRTXTxSSRCNum;
    payload_type = pOfferDetails->uiRTXPayloadNum;
    current_seq_num = pOfferDetails->usRTXSeqNum;
    pOfferDetails->usRTXSeqNum++;
  }else{
    u32SSRC = pOfferDetails->uiTxSSRCNum;
    payload_type = pOfferDetails->uiPayloadNum;
    pOfferDetails->usPrevRcvdSeqNum = current_seq_num;
    pOfferDetails->uiPrevRTPTimestamp  = current_pkt_time;
    current_seq_num = pOfferDetails->usSeqNum + seq_diff;
    current_pkt_time = pOfferDetails->uiTxTimestamp + timestamp_diff;
    pOfferDetails->usSeqNum = current_seq_num;
    pOfferDetails->uiTxTimestamp += timestamp_diff;
  }

  gst_rtp_buffer_set_ssrc (&rtp_buffer, u32SSRC);
  gst_rtp_buffer_set_payload_type (&rtp_buffer, payload_type);

  /*For now no need to modify the sequence num as client re-joining in
   * DTLS connection failure scenario is not handled by client */

  gst_rtp_buffer_set_seq (&rtp_buffer, current_seq_num);
  gst_rtp_buffer_set_timestamp (&rtp_buffer, current_pkt_time);

  #if ((!defined ENABLE_TWCC_TX_PATH) && (defined ENABLE_TWCC_RX_PATH))
    gst_rtp_buffer_remove_extension_data (rtpbuffer);
  #endif

  if (strstr(element_name, "Video") && (m_negotiated_video_delay_playout_ext_Id > 0))
    AddPlayOutDelayExtension (frameRecvBin, &rtp_buffer);

  gst_rtp_buffer_unmap (&rtp_buffer);

  /* 1. Prepare buffer list if number input to function is buffer instead of  buffer list.
   *    if it's a buffer return the list else NULL till buffer list prepared
   *
   * 2. Start of new frame (audio or video) if the time stamp changes w.r.t
   *    to previous RTP time stamp
   *
   * 3. add buffer to buffer list if timestamp difference between current pkt
   *    and previous pkt is 0 i,,e packet of same sample
   *
   * 4. If timestamp differs we can consider it as one complete audio or video
   *    frame and push buffer list to next element
   * */
  if (FALSE == bIsBufferFromList) {
    if (!pOfferDetails->ulNumPktSent ){
      pOfferDetails->pNewBufferList = gst_buffer_list_new ();
      gst_buffer_list_add (pOfferDetails->pNewBufferList, buffer);
      pOfferDetails->pOldBufferList = pOfferDetails->pNewBufferList;
    }else if (timestamp_diff == 0 && pOfferDetails->pOldBufferList){
      gst_buffer_list_add (pOfferDetails->pOldBufferList, buffer);
    }else if(timestamp_diff){
      pOfferDetails->pNewBufferList = gst_buffer_list_new ();
      gst_buffer_list_add (pOfferDetails->pNewBufferList, buffer);
      pBufferList = pOfferDetails->pOldBufferList;
      pOfferDetails->pOldBufferList = pOfferDetails->pNewBufferList;
    }else {
      XGLOG_WARN ("Buffer: %p not added to list ... discarding", buffer);
      gst_buffer_unref (buffer);
      pOfferDetails->ulNumPktSent--;
    }
  }
  pOfferDetails->ulNumPktSent++;
  return pBufferList;
}

void HmpFrameBin::AddPlayOutDelayExtension (HmpFrameBin *dst_frameRecvBin,
                                            GstRTPBuffer *pGstBuffer)
{
  gboolean retVal;
  guint8   DelayValue[3] = {0,0,0};
  guint32  delay = (VIDEO_MIN_PLAYOUT_DELAY_MS << 12) & 0x00FFF000
                    | (VIDEO_MAX_PLAYOUT_DELAY_MS & 0x00000FFF);
  if (m_negotiated_video_delay_playout_ext_Id){
    GST_WRITE_UINT24_BE (DelayValue, delay);
  }
  gst_rtp_buffer_add_extension_onebyte_header (pGstBuffer,
                          m_negotiated_video_delay_playout_ext_Id,
                          DelayValue, sizeof(DelayValue));
}

void HmpFrameBin::on_pad_removed_vid(GstElement *webrtcbin, GstPad *pad, gpointer data)
{
  XGLOG_TRACE( "HmpFrameBin::on_pad_removed_vid Call back called");
}

void HmpFrameBin::on_ice_connection_state_change (GstElement *webrtcbin ,
                                                 GParamSpec * pspec,
                                                 gpointer udata)
{
  Client *client = (Client *)udata;
  ACE_Message_Block* pAmb = NULL;
  HmpFrameBin *frameRecvBin = NULL;
  map<void * , HmpFrameChannel* >::iterator it;
  HmpFrameChannel *frameRecvChannel = NULL;
  guint ice_connection_state = 0;

  g_object_get(G_OBJECT(webrtcbin),"ice-connection-state",&ice_connection_state,NULL);
  XGLOG_INFO( "on_ice_connection_state_change for webrtc: %p ice_connection_state: %d client: %p\n",
      webrtcbin, ice_connection_state, client);

  return;
}

void HmpFrameBin::on_connection_state_change (GstElement *webrtcbin ,
                                              GParamSpec * pspec,
                                              gpointer udata)
{
  Client *client = (Client *)udata;
  ACE_Message_Block* pAmb = NULL;
  HmpFrameBin *frameRecvBin = NULL;
  map<void * , HmpFrameChannel* >::iterator it;
  HmpFrameChannel *frameRecvChannel = NULL;
  guint connection_state = 0;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  frameRecvChannel = client->m_FrameChannel;
  frameRecvBin = frameRecvChannel->m_pHmpFrameBin;

  g_object_get(G_OBJECT(webrtcbin),"connection-state",&connection_state,NULL);
  XGLOG_INFO( "on_connection_state_change for client: %s webrtc: %p dtls_connection_state: %d ",
      client->m_detail.m_callId.c_str(), webrtcbin, connection_state);

  if(connection_state == GST_WEBRTC_PEER_CONNECTION_STATE_FAILED){
    XGLOG_INFO ("GST_WEBRTC_PEER_CONNECTION_STATE_FAILED for webrtc: %s client: %s",
                frameRecvBin->m_webrtc_name.c_str(),
                client->m_detail.m_callId.c_str());
    xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg ();
    ACE_Time_Value tval ((time(NULL) ));
    tval += ACE_Time_Value(0,1);
    pMsg->m_msgType = XGATE_CLIENT_PEER_CONNECTION_FAILED;
    pMsg->param1 = static_cast <void*> (frameRecvChannel);
    pMsg->param2 = static_cast <void*> (client);
    pMsg->pClient = client;
    if (pSFUController->m_ThreadPool_info.m_event_pool) {
      GError *error = NULL;
      g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, &error);
      if (error != NULL){
        XGLOG_ERROR ("Failed to push event to thread pool");
        g_error_free (error);
      }
    }
    else{
      XGLOG_ERROR ("event pool thread is null... can't push event");
    }
  }
  return;
}

void HmpFrameBin::on_ice_gathering_state_change (GstElement *webrtcbin ,
                                                 GParamSpec * pspec,
                                                 gpointer udata)
{
  Client *client = (Client *)udata;
  ACE_Message_Block* pAmb = NULL;
  HmpFrameBin *frameRecvBin = NULL;
  map<void * , HmpFrameChannel* >::iterator it;
  HmpFrameChannel *frameRecvChannel = NULL;
  guint ice_gather_state = 0;
  GstPromise * promise;
  gchar      *sdp_text = NULL;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  g_object_get(G_OBJECT(webrtcbin),"ice-gathering-state",&ice_gather_state,NULL);
  if(GST_WEBRTC_ICE_GATHERING_STATE_COMPLETE != ice_gather_state){
    return;
  }
  frameRecvChannel = client->m_FrameChannel;
  frameRecvBin = frameRecvChannel->m_pHmpFrameBin;
  XGLOG_INFO("on_ice_gathering_state_change webrtc: %p ice_gather_state: %d ",
      webrtcbin, ice_gather_state);

  GstWebRTCSessionDescription *pendingLocalSdp = NULL;
  g_object_get(G_OBJECT(webrtcbin), "local-description", &pendingLocalSdp, NULL);

  if(!pendingLocalSdp || !pendingLocalSdp->sdp) {
    XGLOG_ERROR( "on_ice_candidate_cb pending local sdp is still empty !");
    return;
  }

  sdp_text = gst_sdp_message_as_text(pendingLocalSdp->sdp);
  frameRecvBin->m_localSdpStr.clear();
  frameRecvBin->m_localSdpStr  = sdp_text;
  g_free (sdp_text);
  gst_webrtc_session_description_free(pendingLocalSdp);

  frameRecvBin->m_isLocalSdpCreated = TRUE;
  RegisterRTCPCallback(client);

  //XGLOG_INFO("ice gather state completed sdp generated is\n %s",frameRecvBin->m_localSdpStr.c_str());
  xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg ();

  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  pMsg->m_msgType = XGATE_CLIENT_ICE_CANDIDATE_GENERATED;
  pMsg->param1 = static_cast <void*> (frameRecvChannel);
  pMsg->param2 = static_cast <void*> (client);
  pMsg->pClient = client;
  if (pSFUController->m_ThreadPool_info.m_event_pool) {
    GError *error = NULL;
    g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, &error);
    if (error != NULL){
      XGLOG_ERROR ("Failed to push event to thread pool");
      g_error_free (error);
    }
  }else{
    XGLOG_ERROR ("event pool thread is null... can't push event");
  }
}

void HmpFrameBin::generate_mline_sdp_update_request (Client *pClient,
                                                     PARTICIPANT_LIST *client_list,
                                                     string remote_sdp)
{
  HmpFrameChannel *pFrameChannel = pClient->m_FrameChannel;
  HmpFrameBin *pFrameBin = pFrameChannel->m_pHmpFrameBin;
  mlineofferDetail *offerDetails = NULL;
  ParticipantInfo sPartInfo;
  MLINE_OFFER_MAP::iterator it;
  string prv_callid = "";
  string callId = "";

  for (int participant_index = 0; participant_index < client_list->size(); participant_index++){
    sPartInfo = client_list->at(participant_index);
    /* check if mline is already generated for the participant present in the list */

    it = pClient->m_mlineOffer_map.find (sPartInfo.call_id.c_str());
    if (it != pClient->m_mlineOffer_map.end ()){
       continue;
    }
    if (0 != strcmp (sPartInfo.call_id.c_str (),pClient->m_detail.m_callId.c_str())){
      XGLOG_INFO ("sPartInfo.call_id.c_str :%s pClient->m_detail.m_screenshare_id: %s ",
                      sPartInfo.call_id.c_str(),
                      pClient->m_detail.m_screenshare_id.c_str());
      if (0 != strcmp (sPartInfo.call_id.c_str (),pClient->m_detail.m_screenshare_id.c_str())){
        XGLOG_INFO ("Calling add transceiver for mline dst client: %s",
                         sPartInfo.call_id.c_str());
        pFrameBin->AddTransceivers (pClient, sPartInfo,
                                remote_sdp, GENERATE_ANSWER,
                                GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDONLY);
      }
    }
  }
}

void HmpFrameBin::Check_Inactivate_mline (Client *pClient,
                                          PARTICIPANT_LIST *client_list,
                                          string remote_sdp)
{
  ParticipantInfo sPartInfo;
  mlineofferDetail *offerDetails = NULL;
  MLINE_OFFER_VECTOR deleted_client_list;
  HmpFrameChannel *pFrameChannel = pClient->m_FrameChannel;
  HmpFrameBin *pFrameBin = pFrameChannel->m_pHmpFrameBin;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  string localSDP = "";
  string callId = "";
  gboolean bFound = FALSE;
  XGLOG_INFO ("Entered Check_Inactivate_mline() for callId: %s",
              pClient->m_detail.m_callId.c_str());
  for (int participant_index = pClient->m_mlineOffer_vector.size() -1 ;
           participant_index >= 0;
           participant_index--){
    offerDetails = pClient->m_mlineOffer_vector.at(participant_index);
    /* check if mline is not present in participant list */

    for (int index = 0; index < client_list->size(); index++){
      sPartInfo = client_list->at(index);
      if ((0 == strcmp (sPartInfo.call_id.c_str(),offerDetails->callId_offer_generated.c_str())) ||
                 (strstr (offerDetails->callId_offer_generated.c_str(), EMPTY_DUMMY_CALLID))){
        bFound = TRUE;
        break;
      }
    }
    if (FALSE == bFound){
      XGLOG_INFO ("callId: %s not present in client list",
                           offerDetails->callId_offer_generated.c_str ());
      deleted_client_list.push_back(offerDetails);
      /* Erase the mline offer details from client list */
      pClient->m_mlineOffer_map.erase (offerDetails->callId_offer_generated);
      pClient->m_mlineOffer_vector.erase (pClient->m_mlineOffer_vector.begin() + participant_index);
    }
    /* Reset the variable for next loop */
    bFound = FALSE;
  }
  if (deleted_client_list.size() > 0){
    XGLOG_INFO ("deleted participant list present in callId: %s is size: %d ",
            pClient->m_detail.m_callId.c_str(), deleted_client_list.size());
    for (int index = 0; index < deleted_client_list.size();index++){
      GstPad *Pad = NULL;
      GstWebRTCRTPTransceiver *transceiver;
      offerDetails = deleted_client_list.at (index);
      XGLOG_INFO ("Inactivating m-line in SDP OF callId: %s",
                             offerDetails->callId_offer_generated.c_str ());
      if(offerDetails->eJoinerType != EN_JOINER_TYPE_SCREENSHARE){
      #ifndef AUDIO_MCU_ENABLED
        Pad = gst_element_get_static_pad (pFrameBin->m_webrtcbin,
                        offerDetails->audioOfferDetails.sinkPadName.c_str());
        if (Pad){
          g_object_get (Pad, "transceiver", &transceiver, NULL);
            if (transceiver){
              XGLOG_INFO ("Inactivating transceiver direction for sink pad: %s",
                     offerDetails->audioOfferDetails.sinkPadName.c_str());
              g_object_set (transceiver, "direction",
                     GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_INACTIVE, NULL);
              gst_object_unref (transceiver);
            }
          gst_object_unref (Pad);
          gst_pad_set_active (Pad, false);
        }else {
           XGLOG_ERROR ("audio Pad is null");
        }
      #endif
      Pad = gst_element_get_static_pad (pFrameBin->m_webrtcbin,
                        offerDetails->videoOfferDetails.sinkPadName.c_str());
      if (Pad){
        g_object_get (Pad, "transceiver", &transceiver, NULL);
        if (transceiver){
          XGLOG_INFO ("Inactivating transceiver direction for sink pad: %s",
                    offerDetails->videoOfferDetails.sinkPadName.c_str());
          g_object_set (transceiver, "direction",
                    GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_INACTIVE, NULL);
          gst_object_unref (transceiver);
        }
        gst_object_unref (Pad);
        gst_pad_set_active (Pad, false);
      }else {
        XGLOG_ERROR ("video Pad is null");
      }
      delete offerDetails;
    }
  }
  deleted_client_list.clear();
  }
}

void HmpFrameBin::send_key_frame_request_to_mbc (Client *client, string callId)
{
  xGateMgMsgType eMsgType = EN_XGATE_MG_TYPE_UNKNOWN;
  /* Send KeyFrame Request msg to MBC */
  MgMediaDetail sMediaDetail;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  if (pSFUController){
     sMediaDetail.mgresource_id = client->m_detail.m_mgresourceId;
     sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
     sMediaDetail.joiner_type = client->m_detail.m_joinerType;
     sMediaDetail.call_id = callId;
     XGLOG_INFO( "Sending KeyFrame request to %s for %s",callId.c_str(),client->m_detail.m_callId.c_str());
     eMsgType = EN_XGATE_MG_MEDIA_REQUEST_KEY_FRAME;
    }
    pSFUController->send_msg_sfu_client(&sMediaDetail,client->m_detail,eMsgType);
}

void HmpFrameBin::handle_ice_candidate_msg (HmpFrameBin *pHmpFrameBin,
		                           xGateWebRtcBinType eWebRTCType,
	                            	    Client *client)
{
  xGateMgMsgType eMsgType = EN_XGATE_MG_TYPE_UNKNOWN;
  /* Send offer in msg type 10 to CONF bridge */
  MgMediaDetail sMediaDetail;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  if (pSFUController){
    sMediaDetail.sdp_str = pHmpFrameBin->m_localSdpStr;
    sMediaDetail.mgresource_id = client->m_detail.m_mgresourceId;
    sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
    sMediaDetail.joiner_type = client->m_detail.m_joinerType;
    sMediaDetail.screenshare_id = client->m_detail.m_screenshare_id;
    sMediaDetail.call_id = client->m_detail.m_callId;
    XGLOG_INFO( "Sending answer to client: %s m_webrtcbin: %p joiner_type: %d respnse_msg: %d sdp: %s\n",
                 client->m_detail.m_callId.c_str(),
       	         pHmpFrameBin->m_webrtcbin, sMediaDetail.joiner_type,
       	         pHmpFrameBin->eResponseMsgType,
       	         sMediaDetail.sdp_str.c_str());
    eMsgType = pHmpFrameBin->eResponseMsgType;
  }
  pSFUController->send_msg_sfu_client(&sMediaDetail,client->m_detail,eMsgType);
}

void HmpFrameBin::handle_send_updated_sdp_msg (HmpFrameBin *pHmpFrameBin,
                                               Client *client)
{
  xGateMgMsgType eMsgType = EN_XGATE_MG_TYPE_UNKNOWN;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  GstWebRTCSessionDescription *pendingLocalSdp = NULL;
  mlineofferDetail *mlineDetails = NULL;
  /* Go through the list of m-lines for which details are not populated */
  for (int index = 0; index < client->m_mlineOffer_vector.size (); index++){
     mlineDetails = client->m_mlineOffer_vector.at(index);
     populate_offer_details (client, pHmpFrameBin->m_localSdpStr, mlineDetails);
  }
  MgMediaDetail sMediaDetail;
  if (pSFUController){
    sMediaDetail.sdp_str = pHmpFrameBin->m_localSdpStr;
    sMediaDetail.mgresource_id = client->m_detail.m_mgresourceId;
    sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
    sMediaDetail.joiner_type = client->m_detail.m_joinerType;
    sMediaDetail.call_id = client->m_detail.m_callId;
    eMsgType = EN_XGATE_MG_SDP_UPDATE_RESPONSE;
    XGLOG_INFO( "Sending SDP UPDATE response to client: %s m_webrtcbin: %p sdp: \n %s \n",
                       client->m_detail.m_callId.c_str(),
       	               pHmpFrameBin->m_webrtcbin,
                       sMediaDetail.sdp_str.c_str());
    pSFUController->send_msg_sfu_client(&sMediaDetail,client->m_detail,eMsgType);
  }
}

void HmpFrameBin::populate_offer_details (Client *client,
                                         string sdp_str,
                                         mlineofferDetail *offerDetails)
{
  int pos_r = 0;
  int pos_a = 0;
  int pos_v = 0;
  int pos_a_payload_num = 0;
  int pos_v_payload_num_pos = 0;
  unsigned int payload_num = 0;
  string payload_num_str;
  string ssrc_num_str;
  int delim_pos = 0;
  string::size_type sz;

  guint pos_end = sdp_str.size ();
  if ((!offerDetails) || (TRUE == offerDetails->bPopulated)){
    return;
  }
  ssrc_num_str = to_string(offerDetails->audioOfferDetails.uiTxSSRCNum);
  pos_a = sdp_str.find (ssrc_num_str.c_str());
  ssrc_num_str.erase();
  ssrc_num_str = to_string(offerDetails->videoOfferDetails.uiTxSSRCNum);
  pos_v = sdp_str.find (ssrc_num_str.c_str());
  ssrc_num_str.erase();
  /*find the audio and media line position from pos_end*/
  pos_a = sdp_str.rfind ("m=audio", pos_a);
  pos_v = sdp_str.rfind ("m=video", pos_v);
  XGLOG_INFO ("populating fields OF callId: %s, VideoTx from callId: %s ==> ssrc: %u audio ssrc: %u",
                  client->m_detail.m_callId.c_str(),
                  offerDetails->callId_offer_generated.c_str(),
                  offerDetails->videoOfferDetails.uiTxSSRCNum,
                  offerDetails->audioOfferDetails.uiTxSSRCNum);
  /* find payload, for video rtx*/
  pos_v_payload_num_pos = sdp_str.find ("a=fmtp:", pos_v);
  if(pos_v_payload_num_pos != string::npos){
    delim_pos = sdp_str.find (" apt=", pos_v_payload_num_pos);
    if (delim_pos != string::npos) {
      pos_v_payload_num_pos = sdp_str.rfind (":", delim_pos);
      pos_v_payload_num_pos++;
      payload_num_str = sdp_str.substr (pos_v_payload_num_pos,
                         (delim_pos - pos_v_payload_num_pos));

      try{
        payload_num = std::stoul(payload_num_str,&sz);
      }catch(const std::invalid_argument& ia){
        XGLOG_ERROR("payload_num_str format is incorrect i.e, %s",payload_num_str.c_str());
        payload_num = 0;
      }

    }else {
      payload_num = 0;
      XGLOG_WARN ("apt= line not found.. RTX payload not present in SDP");
    }
    payload_num_str.erase();
  }
  XGLOG_INFO ("video rtx payload_num: %d ",
                       payload_num);
  if (payload_num){
    offerDetails->videoOfferDetails.uiRTXPayloadNum = payload_num;
  }
  /* find SSRC, for video rtx*/
  pos_v_payload_num_pos = sdp_str.find ("a=ssrc-group:FID", pos_v);
  if(pos_v_payload_num_pos != string::npos){
    payload_num = 0;
    delim_pos = sdp_str.find (" " ,pos_v_payload_num_pos);
    delim_pos++;
    pos_v_payload_num_pos = delim_pos;
    delim_pos = sdp_str.find (" " ,pos_v_payload_num_pos);
    delim_pos++;
    pos_v_payload_num_pos = delim_pos;
    delim_pos = sdp_str.find ("\r\n", pos_v_payload_num_pos);
    if (delim_pos != string::npos) {
      ssrc_num_str = sdp_str.substr (pos_v_payload_num_pos,
                        (delim_pos - pos_v_payload_num_pos));
      try{
        payload_num = std::stoul(ssrc_num_str,&sz);
      }catch(const std::invalid_argument& ia){
        XGLOG_ERROR("ssrc_num_str format is incorrect i.e, %s",ssrc_num_str.c_str());
        payload_num = 0;
      }
    }else {
      payload_num = 0;
      XGLOG_WARN ("a=ssrc-group:FID ssrc-group not found in the SDP");
    }
    XGLOG_INFO ("video rtx SSRC : %u ", payload_num);
    ssrc_num_str.erase();
  }
  if (payload_num){
    offerDetails->videoOfferDetails.uiRTXTxSSRCNum = payload_num;
  }
  offerDetails->bPopulated = TRUE;
}

void HmpFrameBin::handle_connection_failed_msg (HmpFrameBin *pHmpFrameBin,
		                           xGateWebRtcBinType eWebRTCType,
	                            	   Client *client)
{
  xGateMgMsgType eMsgType = EN_XGATE_MG_TYPE_UNKNOWN;
  MgMediaDetail sMediaDetail;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  if (pSFUController){
    sMediaDetail.call_id = client->m_detail.m_callId;
    sMediaDetail.mgresource_id = client->m_detail.m_mgresourceId;
    sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
    sMediaDetail.pClient = client;
    XGLOG_INFO( "Posting Ice connection failed message for client: %s ",
                 client->m_detail.m_callId.c_str());
    pSFUController->send_msg_sfu_client(&sMediaDetail,client->m_detail,
                                    EN_XGATE_MG_MEDIA_DTLS_CONNECTION_FAILED);
#if 0
    sMediaDetail.call_id = client->m_detail.m_callId;
    sMediaDetail.mgresource_id = client->m_detail.m_mgresourceId;
    sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
    sMediaDetail.pClient = client;
    XGLOG_INFO( "Posting MEDIA_DELETE message for client: %s ",
       	     client->m_detail.m_callId.c_str());
    pSFUController->send_msg_sfu_client(&sMediaDetail,client->m_detail,
                                    EN_XGATE_MG_MEDIA_DELETE);
#endif
  }
}

const gchar* HmpFrameBin::media_get_ice_ufrag (const GstSDPMessage * msg, guint media_idx)
{
  const gchar *ice_ufrag;

  ice_ufrag = gst_sdp_message_get_attribute_val (msg, "ice-ufrag");
  if (IS_EMPTY_SDP_ATTRIBUTE (ice_ufrag)) {
    const GstSDPMedia *media = gst_sdp_message_get_media (msg, media_idx);
    ice_ufrag = gst_sdp_media_get_attribute_val (media, "ice-ufrag");
    XGLOG_INFO("\n ice_ufrag =%s\n",ice_ufrag);
    if (IS_EMPTY_SDP_ATTRIBUTE (ice_ufrag))
      return NULL;
  }
  return ice_ufrag;
}

const gchar *HmpFrameBin::media_get_ice_pwd (const GstSDPMessage * msg, guint media_idx)
{
  const gchar *ice_pwd;

  ice_pwd = gst_sdp_message_get_attribute_val (msg, "ice-pwd");
  if (IS_EMPTY_SDP_ATTRIBUTE (ice_pwd)) {
    const GstSDPMedia *media = gst_sdp_message_get_media (msg, media_idx);
    ice_pwd = gst_sdp_media_get_attribute_val (media, "ice-pwd");
    XGLOG_INFO("\n ice-pwd==%s\n",ice_pwd);
    if (IS_EMPTY_SDP_ATTRIBUTE (ice_pwd))
      return NULL;
  }
  return ice_pwd;
}

const gchar *HmpFrameBin::media_get_fingerprint (const GstSDPMessage * msg, guint media_idx)
{
  const gchar *fingerprint;

  fingerprint = gst_sdp_message_get_attribute_val (msg, "ice-pwd");
  if (IS_EMPTY_SDP_ATTRIBUTE (fingerprint)) {
    const GstSDPMedia *media = gst_sdp_message_get_media (msg, media_idx);

    const gchar *fingerprint =
      gst_sdp_media_get_attribute_val (media, "fingerprint");

    XGLOG_INFO("\n fingerprint==%s\n",fingerprint);
    if (IS_EMPTY_SDP_ATTRIBUTE (fingerprint))
      return NULL;
  }
  return fingerprint;
}

void HmpFrameBin::get_sdp_attributes(const GstSDPMessage * sdp,
    char **ice_ufrag,
    char **ice_pwd,
    char **fingerprint)
{
  const gchar *message_fingerprint;
  const GstSDPKey *key;
  int i;
  XGLOG_INFO("Inside get_sdp_attributes");
  GstSDPAttribute* attr = gst_sdp_message_get_attribute(sdp,0);
  if(attr != NULL) {
    XGLOG_INFO("\n attr is NULL");
  } else {
    XGLOG_INFO("\n attr is not NULL attr->key=[%s] attr->val=[%s]",attr->key, attr->value);
    XGLOG_INFO( "\n len=%d",gst_sdp_message_medias_len (sdp));
    for (i = 0; i < gst_sdp_message_medias_len (sdp); i++) {
      const GstSDPMedia *media = gst_sdp_message_get_media (sdp, i);
      *ice_ufrag = media_get_ice_ufrag(sdp, i);
      *ice_pwd = media_get_ice_pwd(sdp, i);
      *fingerprint = media_get_fingerprint(sdp, i);
      XGLOG_INFO( "\n *ice_ufrag: %s *ice_pwd: %s",*ice_ufrag, *ice_pwd, *fingerprint);
    }
  }
}

GstCaps *HmpFrameBin::generate_MediaServer_caps(Client *client,
                                                guint media_type,
                                                guint transceiver_direction)
{
  GstCaps *caps = NULL;
  unsigned int u32Index = 0;
  GstCaps *caps1 = NULL;
  GstStructure *gst_struct = NULL;
  gchar *g = NULL;
  guint ssrc = 0;
  GstSDPMessage *sdp = NULL;
  GstStructure *caps_structure;
  gint  payload_num = 0;
  unsigned char final_caps[1000] = {'\0'};
  stringstream caps_string;
  unsigned char u8NegotiatedCodec [20] = {'\0'};
  string final_cap_string;
  unsigned char ssrc_caps[100] = {'\0'};
  HmpFrameBin *pframeRecvBin = this;
  char         *pEncodingParam = NULL;
  gboolean     bAddTwcc = FALSE;
  xGateWebRtcBinType  exGateWebRtcBinType = pframeRecvBin->m_webRtcBinType;
  XGLOG_INFO("Enter generate_MediaServer_caps client: %p webrtcbin: %p webrtcBinType: %d ",
		   client, pframeRecvBin->m_webrtcbin, exGateWebRtcBinType);
  int ret = gst_sdp_message_new (&sdp);
  if(GST_SDP_OK != ret) {
    XGLOG_ERROR( "generate_MediaServer_caps failed to create new gst_sdp_message !");
    return NULL;
  }
  /* get the ssrc */
  if ((transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDONLY)
         || (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDRECV)){
     ssrc = pframeRecvBin->media_get_ssrc(media_type);
     sprintf(ssrc_caps,SSRC_VAL,ssrc);
  }
  /* If media_type is 0, generate caps for audio */
  if(MEDIA_CAPS_FOR_AUDIO == media_type)
  {
    /* If webrtcbin is recvonly type, extract the payload number
     * for the given codec type and store it
     * if webrtcbin is sendonly type use the stored payload numaber
     * and use to generate the offer
     */
    if(0 > m_negotiated_audio_payload_num) {
      payload_num = pframeRecvBin->media_get_payload(m_remoteSdpStr, "opus", NULL);
      XGLOG_INFO("AudioPayload for opus is %d",client->m_audioPayload);
      XGLOG_INFO("payload retrieved from remote sdp for opus is %d", payload_num);
      if(payload_num < 0){
        payload_num = pframeRecvBin->media_get_payload(m_remoteSdpStr, "OPUS", NULL);
        XGLOG_INFO("payload retrieved from remote sdp for OPUS is %d", payload_num);
      }
      if(payload_num > 0){
        m_codec_payload_map.insert({"opus",payload_num});
        m_negotiated_audio_payload_num = payload_num;
        client->m_audioPayload = payload_num;
        strcpy (m_negotiated_audio_codec, "opus");
        strcpy (u8NegotiatedCodec, m_negotiated_audio_codec);
        AudioEncodingParam = "2";
        pEncodingParam = AudioEncodingParam.c_str();
      }
     /* check if twcc is supported and if so, extract the id and save it
      */
     if (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_RECVONLY){
     #ifdef ENABLE_TWCC_RX_PATH
        m_negotiated_audio_twcc_ext_Id
          = pframeRecvBin->extract_ext_id (m_remoteSdpStr, MEDIA_CAPS_FOR_AUDIO, RTP_HDR_EXTN_TWCC);
     #endif
     // #ifndef AUDIO_MCU_ENABLED
        m_negotiated_audio_level_Id
          = pframeRecvBin->extract_ext_id (m_remoteSdpStr, MEDIA_CAPS_FOR_AUDIO, RTP_HDR_EXTN_AUDIO_LEVEL);
    //  #endif
     }
   }else {
     HmpFrameBin *pframeRecvBin = client->m_FrameChannel->m_pHmpFrameBin;
     m_negotiated_audio_payload_num = pframeRecvBin->m_negotiated_audio_payload_num;
     strncpy(u8NegotiatedCodec, pframeRecvBin->m_negotiated_audio_codec,
                                    strlen(pframeRecvBin->m_negotiated_audio_codec));
     pEncodingParam = pframeRecvBin->AudioEncodingParam.c_str();
     m_negotiated_audio_twcc_ext_Id = pframeRecvBin->m_negotiated_audio_twcc_ext_Id;
   }
   if(0 > m_negotiated_audio_payload_num) {
     XGLOG_ERROR ("could not find valid payload num returning NULL caps");
     return NULL;
   }
   caps_string << GENERIC_AUDIO_CAPS;
   if ((transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDONLY)
      || (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDRECV)) {
     caps_string << ssrc_caps;
   }
   caps_string << ", encoding-name=(string)" << u8NegotiatedCodec;
   caps_string << ", payload=(int)" << m_negotiated_audio_payload_num;
   if(pEncodingParam && pEncodingParam[0] != '\0'){
     caps_string << ", encoding-params=(string)";
     caps_string << pEncodingParam;
   }
   /* If webrtc is send only, add caps . for answer, webrtcbin plugin itself^M
    * adds the twcc caps, if client has sent the caps^M
    */
   if (m_negotiated_audio_twcc_ext_Id > 0){
     if (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_RECVONLY){
    #ifdef ENABLE_TWCC_RX_PATH
      caps_string << ", extmap-" << m_negotiated_audio_twcc_ext_Id << "=(string)";
      caps_string << TWCC_EXTMAP_STR;
    #endif
     }else if (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDONLY){
    #ifdef ENABLE_TWCC_TX_PATH
      caps_string << ", extmap-" << m_negotiated_audio_twcc_ext_Id << "=(string)";
      caps_string << TWCC_EXTMAP_STR;
    #endif
      }
    }

    if ((m_negotiated_audio_level_Id > 0)
      && (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_RECVONLY)){
      caps_string << ", extmap-" << m_negotiated_audio_level_Id << "=(string)";
      caps_string << AUDIO_LEVEL_EXTMAP_STR;
    }

    final_cap_string = caps_string.str();
    XGLOG_INFO("ssrc retrieved: %u caps input: %s",ssrc,final_cap_string.c_str());
    caps = gst_caps_from_string (final_cap_string.c_str());
    g = gst_caps_to_string (caps);
    pframeRecvBin->ssrc_audio = ssrc;
    XGLOG_INFO("==== After Merging caps: %s caps: %p ",g,caps);
    final_cap_string.clear ();
    g_free (g);
  }
  /* If media_type is 1, generate caps for video */
  else if(MEDIA_CAPS_FOR_VIDEO == media_type)
  {
    /* If webrtcbin is recvonly type, extract the payload number
     * for the given codec type and store it
     * if webrtcbin is sendonly type use the stored payload numaber
     * and use to generate the offer
     */
    caps_string << GENERIC_VIDEO_CAPS;
    if ((transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDONLY)
      || (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDRECV)) {
      caps_string << ssrc_caps;
    }
    if(0 > m_negotiated_video_payload_num){
       string codec_name = "";
#ifdef USE_VP8_CODEC_CONF
        codec_name = "VP8";
        pEncodingParam = NULL;
#elif defined USE_VP9_CODEC_CONF
        codec_name = "VP9";
        pEncodingParam = NULL;
#else
        codec_name = "H264";
	pEncodingParam = "profile-level-id=42e01f";
	VideoEncodingParam_caps = "level-asymmetry-allowed=(string)1, packetization-mode=(string)1, ";
	VideoEncodingParam_caps += "profile-level-id=(string)42e01f";
	pEncodingParam = VideoEncodingParam_caps.c_str();
#endif
        payload_num = pframeRecvBin->media_get_payload (m_remoteSdpStr, codec_name.c_str(), NULL);
	XGLOG_INFO("payload retrieved from remote sdp for codec: %s is %d", codec_name.c_str(), payload_num);
	if(payload_num > 0){
	   m_codec_payload_map.insert({codec_name,payload_num});
	   m_negotiated_video_payload_num = payload_num;
	   strcpy (m_negotiated_video_codec, codec_name.c_str());
           memset (u8NegotiatedCodec, '\0', sizeof (u8NegotiatedCodec));
           strcpy (u8NegotiatedCodec, codec_name.c_str());
	}

        /*
         * screen share video is not decoded by client due to TWCC
         * sequence numbers are sent in extmap-4 and client is expecting mid value
         * even though TWCC is signalled in extmap 4 field. Disabing TWCC for screen
         * share as workaround
         */
   #ifdef ENABLE_TWCC_RX_PATH
        if ((transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_RECVONLY)
               && (EN_JOINER_TYPE_SCREENSHARE != client->m_detail.m_joinerType)){
           m_negotiated_video_twcc_ext_Id
              = pframeRecvBin->extract_ext_id (m_remoteSdpStr,
                                        MEDIA_CAPS_FOR_VIDEO, RTP_HDR_EXTN_TWCC);
         }
   #endif
   #ifdef ENABLE_VIDEO_PLAYOUT_DELAY
        m_negotiated_video_delay_playout_ext_Id
              = pframeRecvBin->extract_ext_id (m_remoteSdpStr, MEDIA_CAPS_FOR_VIDEO,
                                                 RTP_HDR_EXTN_VIDEO_PLAYOUT_DELAY);
   #endif
    }else {
       HmpFrameBin *pframeRecvBin = client->m_FrameChannel->m_pHmpFrameBin;
       m_negotiated_video_payload_num = pframeRecvBin->m_negotiated_video_payload_num;
       memset (u8NegotiatedCodec, '\0', sizeof (u8NegotiatedCodec));
       strncpy(u8NegotiatedCodec ,pframeRecvBin->m_negotiated_video_codec,
                                    strlen(pframeRecvBin->m_negotiated_video_codec));
       pEncodingParam = pframeRecvBin->VideoEncodingParam_caps.c_str();
       m_negotiated_video_twcc_ext_Id = pframeRecvBin->m_negotiated_video_twcc_ext_Id;
       m_negotiated_video_delay_playout_ext_Id = pframeRecvBin->m_negotiated_video_delay_playout_ext_Id;
    }
    if(0 > m_negotiated_video_payload_num) {
      XGLOG_ERROR ("could not find valid payload num returning NULL caps");
      return NULL;
    }
    caps_string << ", encoding-name=(string)" << m_negotiated_video_codec;
    caps_string << ", payload=(int)" << m_negotiated_video_payload_num;
    if(pEncodingParam && pEncodingParam[0] != '\0'){
       caps_string << ", ";
       caps_string << pEncodingParam;
    }
     /* If webrtc is send only, add caps . for answer, webrtcbin plugin itself^M
      * adds the twcc caps, if client has sent the caps^M
      */
     if (m_negotiated_video_twcc_ext_Id > 0){
        if (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_RECVONLY){
        #ifdef ENABLE_TWCC_RX_PATH
            caps_string << ", extmap-" << m_negotiated_video_twcc_ext_Id << "=(string)";
	    caps_string << TWCC_EXTMAP_STR;
        #endif
           }else if (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDONLY){
        #ifdef ENABLE_TWCC_TX_PATH
            caps_string << ", extmap-" << m_negotiated_video_twcc_ext_Id << "=(string)";
	    caps_string << TWCC_EXTMAP_STR;
        #endif
        }
     }
    /*
     * send video delay playout extension if negotiated
     */
    if ((m_negotiated_video_delay_playout_ext_Id > 0)
         && (transceiver_direction == GST_WEBRTC_RTP_TRANSCEIVER_DIRECTION_SENDONLY)){
       caps_string << ", extmap-" << m_negotiated_video_delay_playout_ext_Id << "=(string)";
       caps_string << VIDEO_PLAYOUT_DELAY_EXTMAP_STR;
    }
    /* Store the ssrc generated to that ssrc can be modified in RTP before sending to other
     * clients
     */
    final_cap_string = caps_string.str();
    XGLOG_INFO("caps input: %s",final_cap_string.c_str());
    caps = gst_caps_from_string (final_cap_string.c_str());
    final_cap_string.clear ();
    /* generate the caps for simulcast based on call direction
     * If it's incoming call,we need to generate answer and in
     * answer send direction becomes recv and recv becomes send
     */
#ifdef SIMULCAST_ENABLE
    if ((exGateWebRtcBinType == EN_WEBRTC_TYPE_SENDRECV )
       || (exGateWebRtcBinType == EN_WEBRTC_TYPE_RECVONLY )){
      if(caps){
        caps_structure = gst_caps_get_structure (caps, 0);
        gst_structure_set (caps_structure,
            "rid:1", G_TYPE_STRING,"recv max-fs=1228800;max-fr=30",
            "rid:2", G_TYPE_STRING,"recv max-fs=614400;max-fr=30",
            "rid:3", G_TYPE_STRING,"recv max-fs=230400;max-fr=30",
            "simulcast",G_TYPE_STRING,"recv 1;2;3",
            NULL);
        gst_caps_append_structure (caps, caps_structure);
      }
    }
#endif
    pframeRecvBin->ssrc_video = ssrc;
    if(caps){
      g = gst_caps_to_string (caps);
      XGLOG_INFO("==== After Merging caps: %s caps: %p",g,caps);
      g_free (g);
    }
  }
  gst_sdp_message_free (sdp);
  return caps;
}

gint HmpFrameBin::media_get_payload (string remote_sdp, char *codec_name, char *encoding_param)
{
  int pos = 0;
  int pos_r = 0;
  string payload_num_str;
  string::size_type sz;
  gint payload_num = -1;
  char codec_string[50];

  /* if codec is H264 and encoding param is not null, find the payload number
   * corresponding to that param. we can get it in a=fmtp line
   */
  strcpy (codec_string, codec_name);
  if((0 == strcmp(codec_name, "H264")) && (NULL != encoding_param ))
    strcpy (codec_string, encoding_param);

  else if (0 == strcmp(codec_name, "VP8")){
    strcat (codec_string, "/90000");
  }

  pos = remote_sdp.find (codec_string,0);
  if(pos != string::npos){
     /* For H264, payload is found in a=fmtp line and for VP8 in a=rtpmap . check if it's for
      * VP8 or H264
      */
    if (0 == strcmp(codec_name, "H264")){
       pos_r = remote_sdp.rfind ("a=fmtp", pos);
    }else {
       pos_r = remote_sdp.rfind ("a=rtpmap", pos);
    }
    if (pos_r != string::npos){
      pos_r = remote_sdp.find (":", pos_r);
      pos_r++;
      pos = remote_sdp.find (" ", pos_r);
      payload_num_str = remote_sdp.substr (pos_r,(pos - pos_r));
      payload_num = std::stoi(payload_num_str,&sz);
    }else {
      XGLOG_ERROR("could not find a=rtpmap to get payload \n");
    }
  }
  return payload_num;
}

string HmpFrameBin:: get_mline_index (string sdp_str, int pos)
{
  int pos_r = -1;
  string mline_str = "";
  string::size_type sz;
  if (pos != string::npos){
    pos = sdp_str.find ("a=mid:", pos);
    if (pos != string::npos){
      pos = pos + strlen ("a=mid:");
      pos_r = sdp_str.find ("\r\n", pos);
      mline_str = sdp_str.substr (pos,(pos_r - pos));
    }else {
       XGLOG_ERROR ("could not find m-line index in sdp ");
    }
  }else {
    XGLOG_ERROR ("pos value passed to get m-line index is wrong");
  }
  return mline_str;
}

gint HmpFrameBin::extract_ext_id (string remote_sdp, guint media_type,
                                  HDR_EXTN_TYPE eHdrExtntype)
{
  GstSDPMessage *pmsg = NULL;
  GstSDPResult SDPResult = GST_SDP_EINVAL;
  const GstSDPMedia *psdpMedia = NULL;
  const GstSDPAttribute* pattrib = NULL;
  int pos = 0;
  int pos_r = 0;
  string::size_type sz;
  gint ext_id = -1;
  int index = 0;

  SDPResult = gst_sdp_message_new_from_text (remote_sdp.c_str(), &pmsg);
  if (GST_SDP_EINVAL == SDPResult){
    XGLOG_ERROR ("Failed to create SDP message from text .. Invalid param");
    return ext_id;
  }
  for (index = 0; index < gst_sdp_message_medias_len (pmsg); index++) {
    psdpMedia = gst_sdp_message_get_media (pmsg, index);
    gchar *media = gst_sdp_media_get_media(psdpMedia);
    XGLOG_INFO ("psdpMedia: %p media_type: %d media: %s", psdpMedia, media_type, media);
    if(!strcmp(gst_sdp_media_get_media(psdpMedia),"audio")
               && (media_type == MEDIA_CAPS_FOR_AUDIO)){
      break;
    }
    else if(!strcmp(gst_sdp_media_get_media(psdpMedia),"video")
              && (media_type == MEDIA_CAPS_FOR_VIDEO)){
      break;
    } else {
      psdpMedia = NULL;
    }
  }
  if(psdpMedia){
    XGLOG_INFO("Number of attributes found in SDP media is %d",
      	      gst_sdp_media_attributes_len(psdpMedia));
    for(index = 0; index < gst_sdp_media_attributes_len(psdpMedia); index++)
    {
      pattrib = gst_sdp_media_get_attribute (psdpMedia, index);
      if(!strcmp(pattrib->key,"extmap")){
        if((eHdrExtntype == RTP_HDR_EXTN_TWCC)
          && strstr(pattrib->value,"transport-wide-cc-extensions")){
          ext_id = get_extension_id(pattrib->value);
          XGLOG_INFO("Found twcc extId: %d", ext_id);
          break;
        }else if((eHdrExtntype == RTP_HDR_EXTN_AUDIO_LEVEL)
          && strstr(pattrib->value,"ssrc-audio-level")){
          ext_id = get_extension_id(pattrib->value);
          XGLOG_INFO("Found audio ssrc level extId: %d", ext_id);
          break;
        }else if((eHdrExtntype == RTP_HDR_EXTN_VIDEO_PLAYOUT_DELAY)
          && strstr(pattrib->value,"playout-delay")){
          ext_id = get_extension_id(pattrib->value);
          XGLOG_INFO("Found playout delay extId: %d", ext_id);
          break;
        }
      }
    }
    gst_sdp_message_free (pmsg);
  }
  return ext_id;
}

void HmpFrameBin::modify_sdp_params(HmpFrameBin *frameRecvBin,
    GstWebRTCSessionDescription *offer)
{
  char *pucIPaddr = NULL;
  int found = 0;
  string replace_str;
  stringstream GstSDP;
  GstSDPConnection *psGstSDPconnectInfo  = NULL;
  GstSDPMedia *psGstSDPMedia = NULL;
  gchar* desc = NULL;

  pucIPaddr = xGateSFUtil::getLocalIpAddr(AF_INET);
  gst_sdp_connection_set(&offer->sdp->connection,"IN","IP4",pucIPaddr,0,0);
  strcpy(offer->sdp->origin.addr,pucIPaddr);
  /* Get the SDP media for audio and then SDP connection info inside the
   * audio line
   */
  psGstSDPMedia = gst_sdp_message_get_media (offer->sdp,0);
  XGLOG_INFO("number of SDP media connections len: %d",
      gst_sdp_media_connections_len (psGstSDPMedia));
  if(0 != gst_sdp_media_connections_len (psGstSDPMedia))
  {
    psGstSDPconnectInfo = gst_sdp_media_get_connection (psGstSDPMedia, 0);
    gst_sdp_connection_set(psGstSDPconnectInfo,"IN","IP4",pucIPaddr,0,0);
  }
  /* Get the SDP media for audio and then SDP connection info inside the
   * audio line
   */
  psGstSDPMedia = gst_sdp_message_get_media (offer->sdp,1);
  XGLOG_INFO("number of SDP media connections len: %d",
      gst_sdp_media_connections_len (psGstSDPMedia));
  if(0 != gst_sdp_media_connections_len (psGstSDPMedia))
  {
    psGstSDPconnectInfo = gst_sdp_media_get_connection (psGstSDPMedia, 0);
    gst_sdp_connection_set(psGstSDPconnectInfo,"IN","IP4",pucIPaddr,0,0);
  }

  desc = gst_sdp_message_as_text(offer->sdp);
  GstSDP << desc;

  /* Copy the offer generated by Gstreamer in framebin */
  frameRecvBin->m_localSdpStr = GstSDP.str();
  XGLOG_INFO( "Modified offer by MG: %s length: %d\n",
      frameRecvBin->m_localSdpStr.c_str(), strlen(desc));
}

guint HmpFrameBin::media_get_ssrc(guint media_idx)
{
  gchar *ssrc_line = NULL;
  guint ssrc= 0;
  ssrc =  RANDOM_SSRC_ID;
  XGLOG_INFO( "Generated ssrc: %u using random generator than from SDP", ssrc);
  return ssrc;
}

/* HmpFrameBin derived class - End
*/

/* HmpVideoChannel bass class - Start
*/
HmpVideoChannel::HmpVideoChannel(string pipelineName, HmpChannelType pipelineType) :
  m_pipelineName(pipelineName), m_pipelineType(pipelineType), m_pipeline(NULL),
  m_bus(NULL), m_busId(0)
{
  m_pipeline = gst_pipeline_new(m_pipelineName.c_str());
  if(!m_pipeline) {
    XGLOG_ERROR( "HmpVideoChannel:: pipeline object creation failed!");
    return;
  }

  m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
}

HmpVideoChannel::~HmpVideoChannel()
{

}

gboolean HmpVideoChannel::register_handle_event_callback(gpointer data)
{
  if(NULL != m_pipeline && NULL != m_bus && NULL != data) {
    m_busId = gst_bus_add_watch(m_bus, &handle_bus_event, data);
    return ((int )m_busId > 0) ? TRUE : FALSE;
  }
  return FALSE;
}

gboolean HmpVideoChannel::handle_bus_event(GstBus* bus, GstMessage* msg, gpointer data)
{
  Client *client = (Client *)data;
  if(!client) {
    XGLOG_ERROR( "HmpVideoChannel::handle_bus_event, invalid client object !!");
    return TRUE;
  }

  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      {
        XGLOG_INFO( "HmpVideoChannel::handle_bus_event got GST_MESSAGE_ERROR message");
        GError *err = NULL;
        gchar *dbgInfo = NULL;
        if(HAVE_GST_DEBUG_PRINTS)
        {
          gst_message_parse_error(msg, &err, &dbgInfo);
          if(err) {
            g_printerr("ERROR from element %s: %s\n",
                        GST_OBJECT_NAME (msg->src), err->message);
            XGLOG_ERROR( "HmpVideoChannel::handle_bus_event ERROR from element %s: %s",
                        GST_OBJECT_NAME(msg->src), err->message);
            g_error_free(err);
          }
          if(dbgInfo) {
            g_printerr("Debugging info: %s\n", (dbgInfo) ? dbgInfo : "none");
            XGLOG_ERROR( "Debugging info: %s", dbgInfo);
            g_free(dbgInfo);
          }
        }
        break;
      }
    case GST_MESSAGE_EOS:
      {
        XGLOG_INFO( "HmpVideoChannel::handle_bus_event got GST_MESSAGE_EOS message");
        break;
      }
    case GST_MESSAGE_ELEMENT:
      {
        //XGLOG_INFO( "HmpVideoChannel::handle_bus_event got GST_MESSAGE_ELEMENT message");
        const GstStructure *gst_struct = gst_message_get_structure (msg);
        const gchar *name = gst_structure_get_name (gst_struct);
        if (strcmp (name, "level") == 0) {
           client->handle_audio_level_msg (client, gst_struct);
        }
        break;
      }
    case GST_MESSAGE_STREAM_STATUS :
      {
         //XGLOG_INFO ("RCVD GST_MESSAGE_STREAM_STATUS client: %s from object: %s",
         //             client->m_detail.m_callId.c_str(), GST_MESSAGE_SRC_NAME (msg));
         //handle_stream_status (msg, data );
         break;
      }
     case GST_MESSAGE_STREAM_START :
      {
         XGLOG_INFO ("RCVD GST_MESSAGE_STREAM_START  client: %s from object: %s",
                      client->m_detail.m_callId.c_str(), GST_MESSAGE_SRC_NAME (msg));
         break;
      }

    case GST_MESSAGE_UNKNOWN:
    default:
      {
        //XGLOG_INFO( "HmpVideoChannel::handle_bus_event got \
        // GST_MESSAGE_UNKNOWN/DEFAULT message %0x", GST_MESSAGE_TYPE(msg));
        break;
      }
  }
  return TRUE; /*it's important to return TRUE from this function */
}

gboolean HmpVideoChannel::draw_dot_file()
{
  gboolean retVal = FALSE;
  if(HAVE_GST_DEBUG_PRINTS && m_pipeline && !m_pipelineName.empty()) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pipeline),
        GST_DEBUG_GRAPH_SHOW_FULL_PARAMS , m_pipelineName.c_str());
    retVal = TRUE;
  }
  return retVal;
}

gboolean HmpVideoChannel::start()
{
  gboolean retVal = FALSE;
  if(m_pipeline) {
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    retVal = TRUE;
  }
  draw_dot_file();
  return retVal;
}

gboolean HmpVideoChannel::stop()
{
  gboolean retVal = FALSE;
  GstStateChangeReturn sret ;
  GstState state;
  GstBus *bus;
  GstClockTime timeout;
  unsigned int retry = 0;
  if(m_pipeline) {
    if(m_bus) {
      gst_bus_set_sync_handler(m_bus, NULL, NULL, NULL);
      gst_object_unref(GST_OBJECT (m_bus));
    }

    if(m_busId) {
       g_source_remove (m_busId);
    }

    XGLOG_INFO("Setting the pipeline: %p to NULL", m_pipeline);
    sret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
    XGLOG_INFO("Setting the pipeline: %p DONE state change: %d ", m_pipeline, sret);

    if (sret == GST_STATE_CHANGE_FAILURE){
       XGLOG_ERROR ("Failed to stop the pipeline ");
       retVal = FALSE;
    }else if (sret == GST_STATE_CHANGE_ASYNC){
       do {
          timeout = 50000 * GST_MSECOND; //nano seconds, 50 milli sec
          sret = gst_element_get_state(GST_ELEMENT (m_pipeline), &state, NULL, &timeout);
          retry++;
       } while (state != GST_STATE_NULL && retry < 20) ;
       if (state == GST_STATE_NULL){
          XGLOG_INFO ("Successfully changed the pipeline state to NULL in retry: %d", retry);
       }else{
          XGLOG_ERROR ("Failed to change pipeline state to NULL after retry: %d times", retry);
       }
    }else if (sret == GST_STATE_CHANGE_SUCCESS){
       XGLOG_INFO ("Successfully changed the pipeline state to NULL");
    }
    gst_object_unref (m_pipeline);
    m_pipeline = NULL;
    retVal = TRUE;
  }
  return retVal;
}

void HmpVideoChannel::handle_stream_status (GstMessage* msg, gpointer data)
{
  GstStreamStatusType StreamStatus;
  GstElement *pElement = NULL;
  Client     *src_client = (Client *) data;
  GstTask    *task = NULL;

  gst_message_parse_stream_status (msg, &StreamStatus, &pElement);
  XGLOG_INFO ("StreamStatus: 0x%x pElement: %p thread_id: %d ",
                  StreamStatus, pElement, syscall(__NR_gettid));
  if (GST_STREAM_STATUS_TYPE_CREATE == StreamStatus && pElement){
    const GValue *val;
    val = gst_message_get_stream_status_object (msg);

    gchar *pElementName = gst_element_get_name (pElement);
    XGLOG_INFO ("Thread started for client: %s Element: %s ",
      src_client->m_detail.m_callId.c_str(), pElementName);

    if (G_VALUE_TYPE (val) == GST_TYPE_TASK) {
      task = g_value_get_object (val);
      XGLOG_INFO ("Element: %s is task: %p", pElementName, task);
    }
    if (pElementName){
      if (strstr (pElementName, NICESRC_ELEMENT_NAME)
         && task){
        struct sched_param param;
        GThread *thread = task->thread;
        if (thread){
          XGLOG_INFO ("nice element task: %p thread_id: %d ", task, syscall(__NR_gettid));
          param.sched_priority = NICESRC_ELEMENT_PRIORITY;

          //pthread_setschedparam (thread, SCHED_RR,&param);
          //pthread_setschedprio (pthread_self (), NICESRC_ELEMENT_PRIORITY);
        }
      }
      g_free (pElementName);
    }
  }
}

/* HmpVideoChannel bass class - End
*/


/* HmpFrameChannel derived class - Start
*/
HmpFrameChannel::HmpFrameChannel(string pipelineName, HmpChannelType pipelineType) :
  HmpVideoChannel(pipelineName, pipelineType), m_pHmpFrameBin(NULL),
  m_pHmpAudioDecoderBin(NULL), m_pHmpAudioEncoderBin(NULL)
{
}

HmpFrameChannel::~HmpFrameChannel()
{
  XGLOG_INFO("HmpFrameChannel::~HmpFrameChannel destruction start");
  if(m_pHmpFrameBin) {
    delete m_pHmpFrameBin;
    m_pHmpFrameBin = NULL;
  }
  if(m_pHmpAudioDecoderBin) {
    delete m_pHmpAudioDecoderBin;
    m_pHmpAudioDecoderBin = NULL;
  }
  if(m_pHmpAudioEncoderBin) {
    delete m_pHmpAudioEncoderBin;
    m_pHmpAudioEncoderBin = NULL;
  }
  XGLOG_INFO("HmpFrameChannel::~HmpFrameChannel destruction ended");
}

gboolean HmpFrameChannel::construct_bin(string binName, HmpVideoBinType vbinType,
                              HmpCodecType codecType, unsigned int dynamicCodec,
                              string sdpStr, HmpCallDir callDir,
                              xGateWebRtcBinType webRtcType, string webrtc_name, Client *client)
{
  if(!m_pHmpFrameBin) {
    m_pHmpFrameBin = new HmpFrameBin(binName, vbinType, codecType,
                         dynamicCodec, sdpStr, callDir, webRtcType, webrtc_name);
    if(!m_pHmpFrameBin->construct(webrtc_name)) {
      XGLOG_ERROR("construct_bin, FrameRecvBin construction failed !!");
      return FALSE;
    } else {
       XGLOG_INFO("Calling add bin m_pipeline: %p m_pHmpFrameBin->m_bin: %p",
          m_pipeline, m_pHmpFrameBin->m_bin);
      gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpFrameBin->m_bin), NULL);
    }
    if (EN_JOINER_TYPE_SCREENSHARE == client->m_detail.m_joinerType){
      XGLOG_INFO ("client: %s joinerType: screen share... not creating decoder and encoder bin",
                        client->m_detail.m_callId.c_str());
      return TRUE;
    }
#ifdef AUDIO_MCU_ENABLED

    if (TRUE != construct_decoder_bin (vbinType, codecType, client)){
      XGLOG_ERROR ("Failed to create decoder bin ");
      return FALSE;
    }

    if (TRUE != construct_encoder_bin (vbinType, codecType, client)){
      XGLOG_ERROR ("Failed to create decoder bin ");
      return FALSE;
    }
#endif
  }
  return TRUE;
}

gboolean HmpFrameChannel::construct_decoder_bin (HmpVideoBinType vbinType,
                                                HmpCodecType codecType,
                                                Client *client )
{
    string decoder_name = "decoder_";
    decoder_name += m_pHmpFrameBin->m_webrtc_name.substr(7, 12);
    string decoder_binName = decoder_name;
    decoder_binName += "bin";
    m_pHmpAudioDecoderBin = new HmpAudioDecoderBin(decoder_binName, vbinType, codecType, decoder_name);
    if(!m_pHmpAudioDecoderBin->construct(decoder_name)) {
      XGLOG_ERROR("construct_bin, AudioDecoderBin construction failed !!");
      return FALSE;
    }else {
      XGLOG_INFO("Calling add bin m_pipeline: %p m_pHmpAudioDecoderBin->m_bin: %p",
                                             m_pipeline, m_pHmpFrameBin->m_bin);
    }
    XGLOG_INFO("construct_bin, AudioDecoderBin construction with decoder_binName:%s and decoder_name:%s",
       decoder_binName.c_str(),decoder_name.c_str());
    decoder_name.clear();
    decoder_binName.clear();

   gst_bin_add(m_pHmpFrameBin->m_bin, GST_ELEMENT(m_pHmpAudioDecoderBin->m_bin));

   gst_element_sync_state_with_parent (GST_ELEMENT(m_pHmpAudioDecoderBin->m_bin));
   return TRUE;
}

gboolean HmpFrameChannel::construct_encoder_bin (HmpVideoBinType vbinType,
                                                HmpCodecType codecType,
                                                Client *client)
{
    string encoder_binName = "";
    string encoder_name = "encoder_";
    encoder_name += m_pHmpFrameBin->m_webrtc_name.substr(7, 12);
    encoder_binName = encoder_name;
    encoder_binName += "bin";
    m_pHmpAudioEncoderBin = new HmpAudioEncoderBin(encoder_binName, vbinType, codecType, encoder_name);
    if(!m_pHmpAudioEncoderBin->create_encoderbin(encoder_name, client)) {
      XGLOG_ERROR("construct_encoder_bin, AudioEncoderBin construction failed !!");
      return FALSE;
    }else {
      XGLOG_INFO("Calling add bin m_pipeline: %p m_pHmpAudioEncoderBin->m_bin: %p",
                                           m_pipeline, m_pHmpFrameBin->m_bin);
    }
    XGLOG_INFO("AudioEncoderBin construction with encoder_binName: %s and encoder_name: %s",
                      encoder_binName.c_str(),encoder_name.c_str());
    encoder_name.clear();
    encoder_binName.clear();

    gst_bin_add(m_pHmpFrameBin->m_bin, GST_ELEMENT(m_pHmpAudioEncoderBin->m_bin));

    gst_element_sync_state_with_parent (GST_ELEMENT(m_pHmpAudioEncoderBin->m_bin));

    return TRUE;
}


gboolean HmpFrameChannel::register_webrtcbin_callback(gpointer data)
{
  Client *client = data;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  if(m_pHmpFrameBin) {
    return m_pHmpFrameBin->register_webrtcbin_callback(data);
  }
  return FALSE;
}

gboolean HmpFrameChannel::handle_sdp_offer(gpointer udata)
{
  if(m_pHmpFrameBin) {
    return m_pHmpFrameBin->handle_sdp_offer(udata);
  }
  return FALSE;
}

gboolean HmpFrameChannel::handle_sdp_answer(gpointer udata)
{
  if(m_pHmpFrameBin) {
    return m_pHmpFrameBin->handle_sdp_answer(udata);
  }
  return FALSE;
}
/*  HmpFrameChannel derived class - End
*/

#ifdef RECORD_ENABLED
/* HmpRecordAudioSendChannel class - Start
*/
HmpRecordAudioSendChannel::HmpRecordAudioSendChannel(string pipelineName, HmpChannelType pipelineType) :
	HmpVideoChannel(pipelineName, pipelineType) ,
	m_pHmpRecAudioSendBin(NULL), m_start(FALSE)
{
}

HmpRecordAudioSendChannel::~HmpRecordAudioSendChannel()
{
  XGLOG_INFO("HmpRecordAudioSendChannel::~HmpRecordAudioSendChannel destruction start");
  m_start = FALSE;
  if(m_pHmpRecAudioSendBin) {
    delete m_pHmpRecAudioSendBin;
    m_pHmpRecAudioSendBin = nullptr;
  }
  XGLOG_INFO("HmpRecordAudioSendChannel::~HmpRecordAudioSendChannel destruction ended");
}

gboolean HmpRecordAudioSendChannel::construct_channel(Client *client)
{
  string id = client->m_detail.m_callId;
  if(!m_pHmpRecAudioSendBin) {
    char *name = "Audio";
    string binName = "audio_rec_send_";
    binName += id.substr(0, 10);
    binName += "_bin";
    m_pHmpRecAudioSendBin = new HmpRecordingSendBin(binName);
    if(!m_pHmpRecAudioSendBin->create_recording_send_bin(name)) {
      XGLOG_ERROR("RecAudioSendBin construction for client '%s' failed !", id.c_str());
      return FALSE;
    }
    XGLOG_INFO( "successfully constructed RecAudioSendBin '%s' for client: '%s'",
				binName.c_str(), id.c_str());
    m_pHmpRecAudioSendBin->m_udpSrcPort = HOSTGST_GET_AUDIO_PORT();
  } else {
    XGLOG_INFO("RecAudioSendBin '%s' already been created for client '%s'",
           m_pHmpRecAudioSendBin->m_binName, id.c_str());
  }

  gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpRecAudioSendBin->m_bin), NULL);
  start();
  return TRUE;
}
/* HmpRecordAudioSendChannel class - End
*/

/* HmpRecordVideoSendChannel class - Start
*/
HmpRecordVideoSendChannel::HmpRecordVideoSendChannel(string pipelineName, HmpChannelType pipelineType) :
	HmpVideoChannel(pipelineName, pipelineType) ,
	m_pHmpRecVideoSendBin(NULL), m_start(FALSE)
{
}

HmpRecordVideoSendChannel::~HmpRecordVideoSendChannel()
{
  XGLOG_INFO("HmpRecordVideoSendChannel::~HmpRecordVideoSendChannel destruction start");
  m_start = FALSE;
  if(m_pHmpRecVideoSendBin) {
    delete m_pHmpRecVideoSendBin;
    m_pHmpRecVideoSendBin = nullptr;
  }
  XGLOG_INFO("HmpRecordVideoSendChannel::~HmpRecordVideoSendChannel destruction ended");
}

gboolean HmpRecordVideoSendChannel::construct_channel(Client *client)
{
  string id = client->m_detail.m_callId;
  if(!m_pHmpRecVideoSendBin) {
    char *name = "Video";
    string binName = "video_rec_send_";
    binName += id.substr(0, 10);
    binName += "_bin";
    m_pHmpRecVideoSendBin = new HmpRecordingSendBin(binName);
    if(!m_pHmpRecVideoSendBin->create_recording_send_bin(name)) {
      XGLOG_ERROR("RecVideoSendBin construction for client: '%s' failed !", id.c_str());
      return FALSE;
    }
    XGLOG_INFO( "successfully constructed RecVideoSendBin '%s' for client: '%s'",
				binName.c_str(), id.c_str());
    m_pHmpRecVideoSendBin->m_udpSrcPort = HOSTGST_GET_AUDIO_PORT();
  } else {
    XGLOG_INFO("RecVideoSendBin '%s' already been constructed for client: '%s'",
				m_pHmpRecVideoSendBin->m_binName, id.c_str());
  }
  gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpRecVideoSendBin->m_bin), NULL);
  start();
  return TRUE;
}
/* HmpRecordVideoSendChannel class - End
*/

/* HmpRecordSendChannel class - Start
*/
HmpRecordSendChannel::HmpRecordSendChannel() :
  m_audioSendChannel(NULL), m_videoSendChannel(NULL), m_clientId(""),
  m_recState(RECORD_STATE_NULL)
{
}

HmpRecordSendChannel::~HmpRecordSendChannel()
{
  XGLOG_INFO("HmpRecordSendChannel::~HmpRecordSendChannel destruction start");
  if(m_videoSendChannel) {
    m_videoSendChannel->stop();
    delete m_videoSendChannel;
    m_videoSendChannel = nullptr;
  }
  if(m_audioSendChannel) {
    m_audioSendChannel->stop();
    delete m_audioSendChannel;
    m_audioSendChannel = nullptr;
  }

  XGLOG_INFO("HmpRecordSendChannel::~HmpRecordSendChannel destruction ended");
}

gboolean HmpRecordSendChannel::construct_record_send_channel(Client *client)
{
  HmpChannelType channelType = EN_PIPELINE_RECORD_FRAME;
  m_clientId = client->m_detail.m_callId;
  string id = client->m_detail.m_callId.substr(0, 10);
  string channelName("");

  if(!m_audioSendChannel) {
    channelName = "Audio_";
    channelName += id;
    channelName += "_channel";
    m_audioSendChannel = new HmpRecordAudioSendChannel(channelName, channelType);
    if(!m_audioSendChannel->construct_channel(client)) {
      XGLOG_ERROR( "RecordAudioSend channel construction for client '%s' failed !", m_clientId.c_str());
      return FALSE;
    }
  }
  XGLOG_INFO( "constructed record audio send channel '%s' for client '%s' success !",
			channelName.c_str(), m_clientId.c_str());

  if(!m_videoSendChannel) {
    channelName = "Video_";
    channelName += id;
    channelName += "_channel";
    m_videoSendChannel = new HmpRecordVideoSendChannel(channelName, channelType);
    if(!m_videoSendChannel->construct_channel(client)) {
      XGLOG_ERROR( "RecordVideoSend channel construction for client: '%s' failed !", m_clientId.c_str());
      return FALSE;
    }
  }
  XGLOG_INFO( "constructed record video send channel '%s' for client '%s' success !",
			channelName.c_str(), m_clientId.c_str());
  return TRUE;
}

void HmpRecordSendChannel::handle_send_recording_msg(Client *client)
{
  xGateMgMsgType eMsgType = EN_XGATE_MG_TYPE_UNKNOWN;
  HmpFrameBin *pframeRecvBin = client->m_FrameChannel->m_pHmpFrameBin;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  MgMediaDetail sMediaDetail;
  if(pSFUController) {
    sMediaDetail.mgresource_id = client->m_detail.m_mgresourceId;
    sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
    sMediaDetail.joiner_type = client->m_detail.m_joinerType;
    sMediaDetail.call_id = client->m_detail.m_callId;
    sMediaDetail.media_event = client->m_detail.m_mediaEvent;
    sMediaDetail.audioDetail.m_mediaType = EN_MEDIA_SDPINFO_AUDIO;
    sMediaDetail.audioDetail.m_dialPort = m_audioSendChannel->m_pHmpRecAudioSendBin->m_udpSrcPort;
    sMediaDetail.videoDetail.m_mediaType = EN_MEDIA_SDPINFO_VIDEO;
    sMediaDetail.videoDetail.m_dialPort = m_videoSendChannel->m_pHmpRecVideoSendBin->m_udpSrcPort;
    sMediaDetail.audioDetail.m_codec = pframeRecvBin->m_negotiated_audio_payload_num;
    sMediaDetail.videoDetail.m_codec = pframeRecvBin->m_negotiated_video_payload_num;
    eMsgType = EN_XGATE_MG_VIDEO_REC_REQ;
    sMediaDetail.record_file = client->m_detail.m_record_file;
    sMediaDetail.browser_type = client->m_detail.m_browser_type;
    XGLOG_INFO("Sending Recording Message to client: %s ", client->m_detail.m_callId.c_str());
    pSFUController->send_msg_sfu_client(&sMediaDetail, client->m_detail, eMsgType);
  }
}

void HmpRecordSendChannel::handle_record_start_res(Client *client)
{
  XGLOG_INFO( "handle_record_start_res for client: %s ",client->m_detail.m_callId.c_str());
  XGLOG_INFO( "Audio m_respIp:%s Audio m_respPort:%d Video m_respIp:%s Video m_respPort:%d",
  client->m_detail.audioDetail.m_respIp.c_str(),
  client->m_detail.audioDetail.m_respPort,
  client->m_detail.videoDetail.m_respIp.c_str(),
  client->m_detail.videoDetail.m_respPort);
  m_audioSendChannel->m_pHmpRecAudioSendBin->m_udpDstPort = client->m_detail.audioDetail.m_respPort;
  m_audioSendChannel->m_pHmpRecAudioSendBin->m_sendIp = client->m_detail.audioDetail.m_respIp;
  m_audioSendChannel->m_pHmpRecAudioSendBin->connect_send_socket();
  m_audioSendChannel->m_pHmpRecAudioSendBin->m_streamId = "Audio";
  m_audioSendChannel->m_pHmpRecAudioSendBin->send_start_event();
  m_audioSendChannel->m_start = TRUE;

  m_videoSendChannel->m_pHmpRecVideoSendBin->m_udpDstPort = client->m_detail.videoDetail.m_respPort;
  m_videoSendChannel->m_pHmpRecVideoSendBin->m_sendIp = client->m_detail.videoDetail.m_respIp;
  m_videoSendChannel->m_pHmpRecVideoSendBin->connect_send_socket();
  m_videoSendChannel->m_pHmpRecVideoSendBin->m_streamId = "Video";
  m_videoSendChannel->m_pHmpRecVideoSendBin->send_start_event();
  m_videoSendChannel->m_start = TRUE;
  m_recState = RECORD_STATE_START;
  generate_key_frame_req(client);
}

void HmpRecordSendChannel::handle_record_resume_res(Client *client)
{
  XGLOG_INFO( "handle_record_resume_res for client: %s ",client->m_detail.m_callId.c_str());
  this->start_record();
  m_recState = RECORD_STATE_START;
  generate_key_frame_req(client);
}

void HmpRecordSendChannel::generate_key_frame_req(Client *rec_client)
{
  string callId;
  xGateHmpConference *pHmpConference = NULL;
  SFUController *pSFUController = xGateSFUtil::getSFUController();

  if(!rec_client || !pSFUController){ return; }

  pHmpConference = pSFUController->find_conference_entry(rec_client->m_detail.m_mgresourceId);

  for (auto itr = pHmpConference->m_clientMap.begin(); itr != pHmpConference->m_clientMap.end(); itr++){
    callId = itr->first;
    if (callId == DUMMY_CALLID)
      continue;
    if (rec_client->m_FrameChannel != nullptr){
      HmpFrameBin *pframeRecvBin = rec_client->m_FrameChannel->m_pHmpFrameBin;
      if(pframeRecvBin != nullptr && callId != DUMMY_CALLID){
        pframeRecvBin->send_key_frame_request_to_mbc(rec_client, callId);
      }
    }
  }
}

gboolean HmpRecordSendChannel::start_record()
{
  if(m_audioSendChannel) {
    m_audioSendChannel->m_start = TRUE;
    //m_audioSendChannel->start();//TODO:Facing issue in sending udp packets for other channels also if stop and start recording channel
  }
  if(m_videoSendChannel) {
    m_videoSendChannel->m_start = TRUE;
    //m_videoSendChannel->start();
  }
  return TRUE;
}

gboolean HmpRecordSendChannel::stop_record()
{
  if(m_audioSendChannel) {
    m_audioSendChannel->m_start = FALSE;
   //m_audioSendChannel->stop();//TODO:Facing issue in sending udp packets for other channels also if stop and start recording channel
  }
  if(m_videoSendChannel) {
    m_videoSendChannel->m_start = FALSE;
   //m_videoSendChannel->stop();
  }
  return TRUE;
}
/*  HmpRecordSendChannel class - End
*/
#endif
