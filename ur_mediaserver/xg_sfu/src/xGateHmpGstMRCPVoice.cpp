
//local include

//self include
#include <gst/gstbuffer.h>
#include <gst/rtp/gstrtcpbuffer.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include "xGateHmpGstBin.h"
#include "xGateSFUtil.h"
#include "xGateHmpGstAudioBin.h"
#include "xGateHmpGstVideoBin.h"
#include "xglog.h"
#include <sys/stat.h>

#define THISMODULE "GstMRCP"

/* HmpBin Base class - Start
*/
//constructor and destuctor
HmpBin::HmpBin(string binName, HmpBinType binType, unsigned int dynamicCodec = 0) :
  m_binName(binName), m_binType(binType), m_bin(NULL),
  m_recvPort(0), m_sendPort(0), m_recvSocket(NULL),
  m_sendSocket(NULL), m_udpRecv(NULL), m_udpSend(NULL),
  m_dynamicCodec(dynamicCodec)
{
  m_bin = GST_BIN(gst_bin_new(binName.c_str()));
}

HmpBin::~HmpBin()
{
  XGLOG_INFO( "HmpBin::~HmpBin start for bin: %s", m_binName.c_str());
}

/*Dummy functions to have backward compatibility */
gboolean HmpBin::draw_dot_file()
{
#if 0
  if(HAVE_GST_DEBUG_PRINTS && m_bin && !m_binName.empty()) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_bin, GST_DEBUG_GRAPH_SHOW_ALL, m_binName.c_str());
    return TRUE;
  }
#endif
  return FALSE;
}

gboolean HmpBin::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
  XGLOG_INFO( "HmpBin::add_client_addr %s:%d start", clientIp.c_str(), clientPort);
  return TRUE;
}


/* HmpChannel bass class - Start
*/
HmpChannel::HmpChannel(string pipelineName, HmpChannelType pipelineType) :
  m_pipelineName(pipelineName), m_pipelineType(pipelineType), m_pipeline(NULL),
  m_bus(NULL), m_busId(0)
{
  XGLOG_INFO( "HmpChannel::HmpChannel construction start");
  m_pipeline = gst_pipeline_new(m_pipelineName.c_str());
  if(!m_pipeline) {
    XGLOG_ERROR( "HmpChannel::HmpChannel, %s pipeline object creation failed!");
    return;
  }

  m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
}

HmpChannel::~HmpChannel()
{
  XGLOG_INFO( "HmpChannel::~HmpChannel destructing Voice channel %s start",
                          m_pipelineName.c_str());
#if 1 //TODO: need to properly dispose commanded for call record testing
  if(m_pipeline) {
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (m_pipeline));
    m_pipeline = NULL;
  }
#endif

  if(m_bus) {
    gst_bus_set_sync_handler(m_bus, NULL, NULL, NULL);
    gst_object_unref(GST_OBJECT (m_bus));
  }

  if(m_busId) {
    g_source_remove (m_busId);
  }
}

gboolean HmpChannel::start()
{
  gboolean retVal = FALSE;
  if(m_pipeline) {
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    retVal = TRUE;
  }
  draw_dot_file();
  return retVal;
}

gboolean HmpChannel::stop()
{
  gboolean retVal = FALSE;
  if(m_pipeline) {
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    retVal = TRUE;
  }
  return retVal;
}
/* HmpChannel bass class - End
*/

gboolean HmpChannel::register_handle_event_callback(gpointer data)
{
  if(m_pipeline && m_bus && data) {
    m_busId = gst_bus_add_watch(m_bus, &handle_event, data);
    return ((int)m_busId > 0) ? TRUE : FALSE;
  }
  return FALSE;
}

gboolean HmpChannel::draw_dot_file()
{
  gboolean retVal = FALSE;
#if 0
  if(HAVE_GST_DEBUG_PRINTS && m_pipeline && !m_pipelineName.empty()) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_pipelineName.c_str());
    retVal = TRUE;
  }
#endif
  return retVal;
}

gboolean HmpChannel::handle_event(GstBus* bus, GstMessage* msg, gpointer data)
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
   case GST_MESSAGE_UNKNOWN:
    default:
      {
        //XGLOG_INFO( "handle_event got GST_MESSAGE_UNKNOWN/DEFAULT message");
        break;
      }
  }
  return TRUE; //it's important to return TRUE from this function
}

/* HmpMRCPVoiceCaptureChannel derived class - Start
*/
HmpMRCPVoiceCaptureChannel::HmpMRCPVoiceCaptureChannel(string pipelineName, HmpChannelType pipelineType) :
  HmpChannel(pipelineName, pipelineType), m_pHmpMRCPVoiceCaptureBin(NULL)
{
}

HmpMRCPVoiceCaptureChannel::~HmpMRCPVoiceCaptureChannel()
{
  XGLOG_INFO( "HmpMRCPVoiceCaptureChannel::~HmpMRCPVoiceCaptureChannel destruction start");
  if(m_pHmpMRCPVoiceCaptureBin) {
    delete m_pHmpMRCPVoiceCaptureBin;
  }
}

gboolean HmpMRCPVoiceCaptureChannel::construct_bin(string binName, HmpBinType binType,xGateHmpClientAsrType asrType, unsigned int audioPayload)
{
  XGLOG_INFO("HmpMRCPVoiceCaptureChannel::construct_bin %s start",binName.c_str());
  if(!m_pHmpMRCPVoiceCaptureBin) {
    m_pHmpMRCPVoiceCaptureBin = new HmpMRCPVoiceCaptureBin(binName, binType,asrType,audioPayload);
    if(!m_pHmpMRCPVoiceCaptureBin->construct()) {
      XGLOG_ERROR( "construct_bin, HmpMRCPVoiceCaptureBin construction failed !!");
      return FALSE;
    } else {
      gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpMRCPVoiceCaptureBin->m_bin), NULL);
    }
  }
  return TRUE;
}

gboolean HmpMRCPVoiceCaptureChannel::add_client_addr(string clientIp,gint port,gboolean addExistingClients){
 if(m_pHmpMRCPVoiceCaptureBin){
   m_pHmpMRCPVoiceCaptureBin->add_client_addr(clientIp,port,addExistingClients);
   return TRUE;
 }
 return FALSE;
}

gboolean HmpMRCPVoiceCaptureChannel::register_buffer_probe_callBack (gpointer data,
                                                                     GstElement *element)
{
  if(m_pHmpMRCPVoiceCaptureBin){
    return m_pHmpMRCPVoiceCaptureBin->register_buffer_probe_callBack(data, element);
 }
 return FALSE;
}

/*  HmpMRCPVoiceCaptureChannel derived class - End
*/

/* HmpMRCPVoiceCaptureBin derived class - Start
*/
HmpMRCPVoiceCaptureBin::HmpMRCPVoiceCaptureBin(string binName,HmpBinType binType,xGateHmpClientAsrType asrType,unsigned int audioPayload):
  HmpBin(binName, binType, audioPayload), m_asrType(asrType), m_audioPayload(audioPayload),m_decJitterBuffer(NULL), m_mrcp_filter(NULL),
  m_rtpDepay(NULL),m_audioResample(NULL),m_audioDecoder(NULL), m_audioConvert(NULL), m_filter(NULL)
{
}

HmpMRCPVoiceCaptureBin::~HmpMRCPVoiceCaptureBin()
{
}

gboolean HmpMRCPVoiceCaptureBin::construct()
{
  if(!create_decode_elements()) {
    XGLOG_ERROR( "construct HmpMRCPVoiceCaptureBin, creating elements failed !!");
    return FALSE;
  }
  if(!add_and_link_elements()) {
    XGLOG_ERROR( "construct HmpMRCPVoiceCaptureBin, add and linking elements failed !!");
    return FALSE;
  }
  return TRUE;
}


gboolean HmpMRCPVoiceCaptureBin::create_decode_elements()
{
  guint latency = 40;
  guint codec_g729 = 0;
  GstCaps *cp,*audioCaps;
  m_mrcp_filter = gst_element_factory_make("capsfilter", NULL);
  audioCaps = gst_caps_from_string("audio/x-raw, channels=(int)1, rate=(int)16000");
#ifndef AUDIO_MCU_ENABLED
  switch(m_binType) {
     case EN_BIN_OPUS_RECV:
      {
        XGLOG_INFO( "create_decode_elements for OPUS_RECV bin");
        m_rtpDepay = gst_element_factory_make("rtpopusdepay", NULL);
        m_audioDecoder = gst_element_factory_make("opusdec", NULL);
        //g_object_set(G_OBJECT(m_audioDecoder), "use-inband-fec", true, NULL);
        //g_object_set(G_OBJECT(m_audioDecoder), "plc", true, NULL);
        break;
      }
    default:
      {
        XGLOG_ERROR( "create_decode_elements failed for Unknown/Unsupported voice capture bin !!");
        return FALSE;
      }
  }
  m_audioConvert = gst_element_factory_make("audioconvert", NULL);
  m_audioResample = gst_element_factory_make("audioresample", NULL);
  m_filter = gst_element_factory_make("capsfilter", NULL);
  string payload = UDP_CAPS_OPUS + to_string(m_audioPayload);
  GstCaps *caps = gst_caps_from_string(payload.c_str());
  XGLOG_INFO( "HmpMRCPVoiceCaptureBin caps opus payload %d!!",m_audioPayload);
  if(caps){
    gchar *Audiocaps = gst_caps_to_string(caps);
    XGLOG_INFO(" Setting audio Caps to VoiceCaptureBin m_mrcp_filter are :%s\n", Audiocaps);
    g_object_set(G_OBJECT(m_mrcp_filter),"caps", caps, NULL);
  }
  if(audioCaps){
    g_object_set(G_OBJECT(m_filter), "caps", audioCaps, NULL);
  }
  gst_caps_unref(caps);
#else
  if(audioCaps){
    g_object_set(G_OBJECT(m_mrcp_filter), "caps", audioCaps, NULL);
  }
#endif
  gst_caps_unref(audioCaps);
  return TRUE;
}

gboolean HmpMRCPVoiceCaptureBin::add_and_link_elements()
{
#ifndef AUDIO_MCU_ENABLED
  if(!m_bin || !m_mrcp_filter || !m_rtpDepay || !m_audioDecoder
    || !m_audioConvert || !m_audioResample || !m_filter ){
    XGLOG_ERROR( "add_and_link_elements failed in HmpMRCPVoiceCaptureBin !!");
    return FALSE;
  }
  gst_bin_add_many(m_bin, m_mrcp_filter, m_rtpDepay, m_audioDecoder, m_audioConvert,
                  m_audioResample, m_filter, NULL);
  if(!gst_element_link_many(m_mrcp_filter, m_rtpDepay, m_audioDecoder, m_audioConvert,
                            m_audioResample, m_filter, NULL)){
    XGLOG_ERROR( "HmpMRCPVoiceCaptureBin->add_and_link_elements: failed to link elements!!\n");
    return FALSE;
  }
#else
  if(!m_bin || !m_mrcp_filter ){
    XGLOG_ERROR( "add_and_link_elements failed in HmpMRCPVoiceCaptureBin !!");
    return FALSE;
  }
  gst_bin_add_many(m_bin, m_mrcp_filter, NULL);
#endif
  return TRUE;
}

gboolean HmpMRCPVoiceCaptureBin::register_buffer_probe_callBack (gpointer data, GstElement *element)
{
 gulong probeId = 0;
 GstPad *srcPad = NULL;
 if(element){
   srcPad = gst_element_get_static_pad (element, "src");
   if (srcPad){
     probeId = gst_pad_add_probe (srcPad,(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_PUSH ),
                                handle_PadProbeCallback, data, NULL);
     XGLOG_INFO("register_buffer_probe_callBack generated buffer probe_Id: %lu ", probeId);
     return TRUE;
   } else {
     XGLOG_ERROR ("Failed to get src pad for capsfilter element");
   }
 }
 return FALSE;
}

bool send_binary_data(const uint8_t* data, size_t size,
#ifdef WS_CONN_URL
  SSL *m_ssl_context
#else
  int sockfd
#endif
) {
  std::string frame;
  frame += '\x82';  // FIN bit set, binary frame (opcode 0x2)

  frame += '\x80' | '\x7E'; // Extended payload length indicator with MASK bit set
  frame += static_cast<char>((size >> 8) & 0xFF); // 2 bytes length
  frame += static_cast<char>(size & 0xFF);

  // Generate a random 4-byte masking key
  uint8_t masking_key[4];
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  for (int i = 0; i < 4; ++i) {
    masking_key[i] = std::rand() % 256;
    frame += masking_key[i];
  }

  // Mask the data
  std::vector<uint8_t> masked_data(data, data + size);
  for (size_t i = 0; i < size; ++i) {
    masked_data[i] ^= masking_key[i % 4];  // Apply the mask key
  }

  frame.append(reinterpret_cast<const char*>(masked_data.data()), size); // Append the masked payload

  // Send the frame
  int result;
#ifdef WS_CONN_URL
  result = SSL_write(m_ssl_context, frame.c_str(), frame.size());
  if (result <= 0) {
    int error_code = SSL_get_error(m_ssl_context, result);
    XGLOG_ERROR("Error writing binary frame to SSL socket: %d",error_code);
    ERR_print_errors_fp(stderr);
    perror("Send error");
    XGLOG_ERROR("Failed to send data. Reason: %s ", strerror(errno));
    return false;
  }
#else
  result = send(sockfd, frame.c_str(), frame.size(), 0);
  if (result <= 0) {
    XGLOG_ERROR("Error writing binary frame to TCP socket.");
    perror("Send error");
    XGLOG_ERROR("Failed to send data. Reason: %s ", strerror(errno));
    return false;
  }
#endif

  return true;
}
GstPadProbeReturn HmpMRCPVoiceCaptureBin::handle_PadProbeCallback (GstPad * pad,
                                                         GstPadProbeInfo * ProbeInfo,
                                                         gpointer data)
{
  Client *client = (Client*)data;
  GstBuffer *buffer = NULL;
  GstMapInfo info;
  static int count = 0;
  buffer = gst_pad_probe_info_get_buffer (ProbeInfo);
  if (!buffer){
    XGLOG_ERROR ("Buffer is empty ");
    goto return_from_func;
  }
  if(client->m_detail.m_asrType == EN_XGATE_ASR_INHOUSE)
  {
    if(client && client->m_inhouseClient && client->m_inhouseClient->getIsRecogProcessing())
    {
      gst_buffer_map(buffer,&info,GST_MAP_READ);
#ifdef WS_CONN_URL
      if (!send_binary_data(info.data, info.size, client->m_inhouseClient->m_ssl_context)) {
#else
      if (!send_binary_data(info.data, info.size, client->m_inhouseClient->m_sockfd)) {
#endif
        XGLOG_ERROR("HmpMRCPVoiceCaptureBin::handle_PadProbeCallback Failed to buffer via WebSocket.");
        if(client->m_pHmpMRCPVoiceCaptureChannel){
          XGLOG_INFO("Stopping Voice capture channel");
          client->m_pHmpMRCPVoiceCaptureChannel->stop();
          delete client->m_pHmpMRCPVoiceCaptureChannel;
          client->m_pHmpMRCPVoiceCaptureChannel = NULL;
        }

        if(client->restartRecognization() == EN_XGATE_STATUS_SUCCESS)
        {
          XGLOG_INFO("HmpMRCPVoiceCaptureBin::handle_PadProbeCallback Recognization restarted for connection id(%s)",
                              client->m_detail.m_conn_id.c_str());
        }
      }
      gst_buffer_unmap(buffer,&info);
    }
  }
  gst_buffer_unref (buffer);
return_from_func:
  return GST_PAD_PROBE_HANDLED ;
}
/* HmpMRCPVoiceCaptureBin derived class - End */

