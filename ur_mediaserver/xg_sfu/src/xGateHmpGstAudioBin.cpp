
//local include

//self include
#include <gst/gstbuffer.h>
#include <gst/rtp/gstrtpbuffer.h>
#include <gst/rtp/gstrtcpbuffer.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include "xGateHmpGstBin.h"
#include "xGateSFUtil.h"
#include "xGateHmpGstAudioBin.h"
#include "xglog.h"
#include <sys/stat.h>

#define THISMODULE "GstAudio"

static int count1 = 0;

static gboolean user_function(GstElement* object,GstBuffer* arg0,gpointer user_data)
{
	count1++;
	return TRUE;
}

/* HmpBin Base class - Start
*/
//constructor and destuctor
HmpBin::HmpBin(string binName, HmpBinType binType, unsigned int dynamicCodec = 0) : 
  m_binName(binName), m_binType(binType), m_bin(NULL),
  m_recvPort(0), m_sendPort(0), m_audioSocket(NULL), m_videoSocket(NULL), 
  m_sendSocket(NULL), m_udpRecv(NULL), m_udpSend(NULL),
  m_dynamicCodec(dynamicCodec) 
{
  m_bin = GST_BIN(gst_bin_new(binName.c_str()));
}

HmpBin::~HmpBin()
{
  XGLOG_INFO("HmpBin::~HmpBin start for bin: %s", m_binName.c_str());
}

gboolean HmpBin::create_udpsrc()
{
  XGLOG_INFO( "create_udpsrc start");
  if(!m_udpRecv) {
    m_udpRecv = gst_element_factory_make("udpsrc", NULL);
    if(m_udpRecv) {
      if(m_recvSocket) {
        g_object_set(G_OBJECT(m_udpRecv), "socket", m_recvSocket, NULL);
      } else {
        XGLOG_ERROR( "create_udpsrc failed due to invalid udp socket !!"); 
        return FALSE;
      }

      GstCaps* caps = get_caps();
      if(caps) {
        g_object_set(G_OBJECT(m_udpRecv), "caps", caps, NULL);
        gst_caps_unref(caps);
      } else {
        XGLOG_ERROR( "create_udpsrc setting caps for udpsrc failed !!"); 
        return FALSE;
      }
      g_object_set(G_OBJECT(m_udpRecv), "timeout", 0, "reuse", TRUE, "close-socket", FALSE, NULL);
    } else {
      XGLOG_ERROR( "create_udpsrc gst_element_make udpsrc failed !!");
      return FALSE;
    }
  }
  return TRUE;
}

gboolean HmpBin::create_udpsink()
{
  XGLOG_INFO( "HmpBin::create_udpsink start");
  gint64 maxLateness = 40000000;
  if(!m_udpSend) {
    m_udpSend = gst_element_factory_make("multiudpsink", NULL);
    if(m_udpSend) {
      if(m_sendSocket) {
        g_object_set(G_OBJECT(m_udpSend), "socket", m_sendSocket, NULL);
      } else {
        XGLOG_ERROR( "create_udpsink, failed due to invalid udp socket !!"); 
        return FALSE;
      }
      g_object_set(G_OBJECT(m_udpSend),"async", FALSE, \
          "sync", TRUE, "close-socket", FALSE, "send-duplicates", FALSE, NULL);
      g_object_set(G_OBJECT(m_udpSend),"max-lateness", maxLateness, NULL);
    } else {
      XGLOG_ERROR( "create_udpsink, gst_element_make multiudpsink failed !!");
      return FALSE;
    }
  }
  return TRUE;
}

gboolean HmpBin::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
  XGLOG_INFO( "HmpBin::add_client_addr %s:%d start", clientIp.c_str(), clientPort);
  if(m_udpSend) {
    string allClientAddrs("");
    gchar *existingClientAddr;
    char clientAddr[40] = {0};

    snprintf(clientAddr, 40, "%s:%d", clientIp.c_str(), clientPort);
    if(addExistingClients) {
      /* Getting existing client from m_udpSend */
      g_object_get(G_OBJECT(m_udpSend),"clients", &existingClientAddr, NULL);
      if(existingClientAddr[0] != 0) {
        if(0 == strcmp((char *)existingClientAddr, clientAddr)) {
          XGLOG_INFO( "HmpBin::add_client_addr already has this address : %s in Socket ", existingClientAddr);
          return TRUE;
        }
        allClientAddrs.append(existingClientAddr);
        allClientAddrs += ",";
      }
      g_free(existingClientAddr);
      XGLOG_INFO( "HmpBin::add_client_addr Having existing address : %s in Socket ", allClientAddrs.c_str());
    }
    allClientAddrs.append(clientAddr);

    g_object_set(G_OBJECT(m_udpSend),"clients", allClientAddrs.c_str(), NULL);
    XGLOG_INFO( "HmpBin::add_client_addr New address %s is setted in udp socket ", allClientAddrs.c_str());
    return TRUE;
  } else {
    XGLOG_ERROR( "HmpBin::add_client_addr Not able to set new address %s:%d in udp socket ", clientIp.c_str(), clientPort);
    return FALSE;
  }
  return TRUE;
}

gboolean HmpBin::clear_client_addr()
{
  if(m_udpSend) {
    char clientAddr[40] = {0};
    snprintf(clientAddr, 40, "%s", "0.0.0.0:0");   
    g_object_set(G_OBJECT(m_udpSend),"clients", clientAddr, NULL); 
  } else return FALSE;

  return TRUE; 
}

GstCaps * HmpBin::get_caps()
{
  GstCaps *caps = NULL;
  string udp_caps_opus(UDP_CAPS_OPUS);
  string codec_type = to_string(m_dynamicCodec);
  udp_caps_opus = udp_caps_opus+codec_type;

  switch (m_binType) {
    case EN_BIN_G711U_RECV:
      {
        XGLOG_INFO( "HmpBin::get_caps for G711U_RECV bin from UDP_CAPS_G711U string");
        caps = gst_caps_from_string(UDP_CAPS_G711U);  
        break;
      }
    case EN_BIN_G711A_RECV:
      {
        XGLOG_INFO( "HmpBin::get_caps for G711A_RECV bin from UDP_CAPS_G711A string");
        caps = gst_caps_from_string(UDP_CAPS_G711A);  
        break;
      }
    case EN_BIN_G722_RECV:
      {
        XGLOG_INFO( "HmpBin::get_caps for G722_RECV bin from UDP_CAPS_G722 string");
        caps = gst_caps_from_string(UDP_CAPS_G722);  
        break;
      }
    case EN_BIN_DTLS_RECV:
      {
        XGLOG_INFO( "HmpBin::get_caps for DTLS_RECV bin from UDP_CAPS_DTLS string");
        caps = gst_caps_from_string(UDP_CAPS_DTLS);  
        break;
      }
    case EN_BIN_OPUS_RECV:
      {
        XGLOG_INFO( "HmpBin::get_caps for OPUS_RECV bin from UDP_CAPS_OPUS string");
        caps = gst_caps_from_string(udp_caps_opus.c_str());  
        break;
      }
    case EN_BIN_G729_RECV:
      {
        XGLOG_INFO( "HmpBin::get_caps for G729_RECV bin from UDP_CAPS_G729 string");
        caps = gst_caps_from_string(UDP_CAPS_G729);  
        break;
      }
    case EN_BIN_DTLS_SEND:
      {
        XGLOG_INFO( "HmpBin::get_caps for DTSL_SEND bin from UDP_CAPS_OPUS string");
        caps = gst_caps_from_string(udp_caps_opus.c_str());  
        break;
      }
    case EN_BIN_G711U_SEND:
    case EN_BIN_G711A_SEND:
    case EN_BIN_G722_SEND:
    case EN_BIN_OPUS_SEND:
    case EN_BIN_G729_SEND:
    case EN_BIN_LOCAL_AUDIO_SEND:
    case EN_BIN_LOCAL_AUDIO_RECV:
      {
        XGLOG_INFO( "HmpBin::get_caps for SEND bin from string UDP_CAPS_L16");
        caps = gst_caps_from_string(UDP_CAPS_L16);  
        break;
      }
    default:
      {
        XGLOG_INFO( "HmpBin::get_caps failed for Unknown/Unsupported bin");
        break;
      }
  }
  return caps;
}

gboolean HmpBin::draw_dot_file()
{
#if 1
  if(HAVE_GST_DEBUG_PRINTS && m_bin && !m_binName.empty()) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_bin, GST_DEBUG_GRAPH_SHOW_ALL, m_binName.c_str());
    return TRUE;
  }
#endif
  return FALSE;
}
/* HmpBin Base class - End
*/

/* HmpRecordAudioBin derived class - Start
*/
HmpRecordAudioBin::HmpRecordAudioBin(string binName, HmpBinType binType, GSocket *sendSocket) :
  HmpBin(binName, binType)
{
  m_sendSocket = sendSocket;
  XGLOG_INFO( "HmpRecordAudioBin::HmpRecordAudioBin construction start");
}

HmpRecordAudioBin::~HmpRecordAudioBin()
{
  XGLOG_INFO( "HmpRecordAudioBin::~HmpRecordAudioBin destruction start");
  m_sendSocket = NULL;
}

gboolean HmpRecordAudioBin::construct()
{
  XGLOG_INFO( "HmpRecordAudioBin::construct start");
  while(!m_recvSocket) {
    m_recvPort = HOSTGST_GET_LOOPBACK_PORT();
    m_recvSocket = xGateHmpGstUtil::create_udpsocket(m_recvPort);
  }

  if(!create_udpsrc()) {
    XGLOG_ERROR( "construct HmpRecordAudioBin, creating udpsrc failed !!");
    return FALSE;
  }

  //create udpsink to send packets to client
  if(!create_udpsink()) {
    XGLOG_ERROR( "construct HmpRecordAudioBin, creating udpsink failed !!");
    return FALSE;
  }

  //add and link elements in bin container
  if(!add_and_link_elements()) {
    XGLOG_ERROR( "construct HmpRecordAudioBin, add and linking elements failed !!");
    return FALSE;
  }

  return TRUE;
}

gboolean HmpRecordAudioBin::add_and_link_elements()
{
  XGLOG_INFO( "HmpRecordAudioBin::add_and_link_elements start");
  switch(m_binType) {
    case EN_BIN_G711U_SEND:
    case EN_BIN_G711A_SEND:
    case EN_BIN_G722_SEND:
    case EN_BIN_G729_SEND:
      {
        gst_bin_add_many(m_bin, m_udpRecv, m_udpSend, NULL);
        gst_element_link_many(m_udpRecv, m_udpSend, NULL);
        break;
      }
    default:
      {
        XGLOG_INFO( "add_and_link_elements failed for Unknown/Unsupported voice recv bin");
        break;
      }
  }
  return TRUE;
}

/* HmpRecordAudioBin derived class - End
*/

/* HmpChannel bass class - Start
*/
HmpChannel::HmpChannel(string pipelineName, HmpChannelType pipelineType) :
  m_pipelineName(pipelineName), m_pipelineType(pipelineType), 
	m_pipeline(NULL), m_bus(NULL), m_busId(0) 
{
  XGLOG_INFO("HmpChannel::HmpChannel construction start");
  m_pipeline = gst_pipeline_new(m_pipelineName.c_str());
  if(!m_pipeline) {
    XGLOG_ERROR( "HmpChannel::HmpChannel, %s pipeline object creation failed!");
    return;
  }

  m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
}

HmpChannel::~HmpChannel()
{
  XGLOG_INFO("HmpChannel::~HmpChannel destructing %s start", m_pipelineName.c_str());
#if 0 //TODO: need to properly dispose commanded for call record testing
  if(m_pipeline != NULL) {
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

gboolean HmpChannel::register_handle_event_callback(gpointer data)
{
  if(m_pipeline && m_bus && data) {
    m_busId = gst_bus_add_watch(m_bus, &handle_event, data); 
    return ((int)m_busId > 0) ? TRUE : FALSE;
  } 
  return FALSE;
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
		case GST_MESSAGE_EOS:
			{
				string &playFile = client->m_detail.audioDetail.m_playFile;
				XGLOG_INFO( "handle_event got GST_MESSAGE_EOS message file play_file:%s", playFile.c_str());
				xGateMediaEvent mediaEvent = client->m_detail.m_mediaEvent;
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

gboolean HmpChannel::draw_dot_file()
{
  gboolean retVal = FALSE;
  if(HAVE_GST_DEBUG_PRINTS && m_pipeline && !m_pipelineName.empty()) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_pipelineName.c_str());
    retVal = TRUE;
  }
  return retVal;
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
	//draw_dot_file();
	return retVal;
}
/* HmpChannel bass class - End
*/

/* HmpRecordAudioChannel derived class - Start
*/
HmpRecordAudioChannel::HmpRecordAudioChannel(string pipelineName, HmpChannelType pipelineType) :
  HmpChannel(pipelineName, pipelineType), m_pHmpRecordAudioBin(NULL)
{
  XGLOG_INFO( "HmpRecordAudioChannel::HmpRecordAudioChannel construction start");
}

HmpRecordAudioChannel::~HmpRecordAudioChannel()
{
  XGLOG_INFO( "HmpRecordAudioChannel::~HmpRecordAudioChannel destruction start");
  if(m_pHmpRecordAudioBin) {
    delete m_pHmpRecordAudioBin;
    m_pHmpRecordAudioBin = NULL;
  }
}

gboolean HmpRecordAudioChannel::construct_bin(string binName, HmpBinType binType, GSocket *sendSocket)
{
  XGLOG_INFO( "HmpRecordAudioChannel::construct_bin %s start", binName.c_str());
  if(!m_pHmpRecordAudioBin) {
    m_pHmpRecordAudioBin = new HmpRecordAudioBin(binName, binType, sendSocket); 
    if(!m_pHmpRecordAudioBin->construct()) {
      XGLOG_ERROR( "construct_bin, HmpRecordAudioBin construction failed !!");
      return FALSE;
    } else {
      gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpRecordAudioBin->m_bin), NULL); 
    }
  }  
  return TRUE;
}

gint HmpRecordAudioChannel::get_recv_port()
{
  gint recvPort = 0;
  if(m_pHmpRecordAudioBin) {
    XGLOG_INFO( "HmpRecordAudioChannel::get_recv_port %d" , m_pHmpRecordAudioBin->m_recvPort);
    recvPort = m_pHmpRecordAudioBin->m_recvPort;
  }
  return recvPort;
}

gboolean HmpRecordAudioChannel::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
  if(m_pHmpRecordAudioBin) {
    m_pHmpRecordAudioBin->m_sendIp = clientIp;
    m_pHmpRecordAudioBin->m_sendPort = clientPort;
    m_pHmpRecordAudioBin->add_client_addr(clientIp, clientPort, addExistingClients);
    return TRUE;
  }
  return FALSE;
}

gboolean HmpRecordAudioChannel::clear_client_addr()
{
  if(m_pHmpRecordAudioBin) {
    m_pHmpRecordAudioBin->clear_client_addr();
    return TRUE;
  }
  return FALSE;
}
/* HmpRecordAudioChannel derived class - End
*/

#if 0
/* HmpLocalAudioRecvBin derived class - Start
*/
HmpLocalAudioRecvBin::HmpLocalAudioRecvBin(string binName, HmpBinType binType) :
  HmpBin(binName, binType), m_rtpL16depay(NULL), m_audioConvert(NULL), 
  m_ghostSrcPad(NULL)
{
}

HmpLocalAudioRecvBin::~HmpLocalAudioRecvBin()
{
}

gboolean HmpLocalAudioRecvBin::construct()
{
  //allocate loopback port and create udpsrc to receive internal packets
  while(!m_recvSocket) {
    m_recvPort = HOSTGST_GET_LOOPBACK_PORT();
    m_recvSocket = xGateHmpGstUtil::create_udpsocket(m_recvPort);
  }

  if(!create_udpsrc()) {
    XGLOG_ERROR( "construct HmpLocalAudioRecvBin, creating udpsrc failed !!");
    return FALSE;
  }

  if(!create_elements()) {
    XGLOG_ERROR( "construct HmpLocalAudioRecvBin, creating elements failed !!");
    return FALSE;
  }

  if(!add_and_link_elements()) {
    XGLOG_ERROR( "construct HmpLocalAudioRecvBin, add and linking elements failed !!");
    return FALSE;
  }

  return TRUE;
}

gboolean HmpLocalAudioRecvBin::create_elements()
{
  XGLOG_INFO( "HmpLocalAudioRecvBin::create_elements start");

  m_rtpL16depay = gst_element_factory_make("rtpL16depay", NULL);
  m_audioConvert = gst_element_factory_make("audioconvert", NULL);

  return TRUE;
}

gboolean HmpLocalAudioRecvBin::add_and_link_elements()
{
  if(!m_bin || !m_udpRecv || !m_rtpL16depay || !m_audioConvert) {
    XGLOG_ERROR( "add_and_link_elements failed. elements not creaed properly !!");
    return FALSE;
  }
  gst_bin_add_many(m_bin, m_udpRecv, m_rtpL16depay, m_audioConvert, NULL);

  GstPad *pad = gst_element_get_static_pad (m_audioConvert, "src");
  m_ghostSrcPad = gst_ghost_pad_new(NULL, pad);
  if(pad && m_ghostSrcPad) {
    gst_element_add_pad(GST_ELEMENT(m_bin), m_ghostSrcPad);
    gst_ghost_pad_set_target((GstGhostPad*)m_ghostSrcPad, pad);
    gst_object_unref (GST_OBJECT (pad));
  } else {
    XGLOG_ERROR( "add_and_link_elements failed. adding and linking ghost pad failed !!");
    return FALSE;
  }

  return gst_element_link_many(m_udpRecv, m_rtpL16depay, m_audioConvert, NULL);
}
/* HmpLocalAudioRecvBin derived class - End
*/

/* HmpLocalAudioSendBin derived class - Start
*/
HmpLocalAudioSendBin::HmpLocalAudioSendBin(string binName, HmpBinType binType) :
  HmpBin(binName, binType), m_audioRate(NULL), m_audioConvert(NULL), m_rtpL16pay(NULL), m_ghostSinkPad(NULL)
{
}

HmpLocalAudioSendBin::~HmpLocalAudioSendBin()
{
}

gboolean HmpLocalAudioSendBin::construct()
{
  /*allocate loopback port and updsink for internal packet routing */
  while(!m_sendSocket) {
    m_sendPort = HOSTGST_GET_LOOPBACK_PORT();
    m_sendSocket = xGateHmpGstUtil::create_udpsocket(m_sendPort);
  }
  if(!create_udpsink()) {
    XGLOG_ERROR( "construct HmpLocalAudioSendBin, creating udpsink failed !!");
    return FALSE;
  }

  if(!create_elements()) {
    XGLOG_ERROR( "construct HmpLocalAudioSendBin, creating elements failed !!");
    return FALSE;
  }
  if(!add_and_link_elements()) {
    XGLOG_ERROR( "construct HmpLocalAudioSendBin, add and linking elements failed !!");
    return FALSE;
  }

  return TRUE;
}

gboolean HmpLocalAudioSendBin::create_elements()
{
  XGLOG_INFO( "HmpLocalAudioSendBin::create_elements start");

  //  m_audioRate = gst_element_factory_make("audiorate", NULL);
  m_audioRate = gst_element_factory_make("audioresample", NULL);
  m_audioConvert = gst_element_factory_make("audioconvert", NULL);

  m_rtpL16pay = gst_element_factory_make("rtpL16pay", NULL);

  if(!m_audioRate || !m_audioConvert || !m_rtpL16pay) {
    XGLOG_ERROR( "create_elements, creating gst elements failed !!");
    return FALSE;
  }

  return TRUE;
}

gboolean HmpLocalAudioSendBin::add_and_link_elements()
{
  if(!m_bin || !m_audioRate || !m_audioConvert || !m_rtpL16pay || !m_udpSend) {
    XGLOG_ERROR( "add_and_link_elements failed. elements not creaed properly !!");
    return FALSE;
  }
  gst_bin_add_many(m_bin, /*m_audioRate,*/ m_audioConvert, m_rtpL16pay, m_udpSend, NULL);

  //  GstPad *pad = gst_element_get_static_pad (m_audioRate, "sink");
  GstPad *pad = gst_element_get_static_pad (m_audioConvert, "sink");
  m_ghostSinkPad = gst_ghost_pad_new("sink", pad);
  if(pad && m_ghostSinkPad) {
    gst_element_add_pad(GST_ELEMENT(m_bin), m_ghostSinkPad);
    gst_pad_set_active(m_ghostSinkPad, TRUE);
    gst_object_unref (GST_OBJECT (pad));
  } else {
    XGLOG_ERROR( "add_and_link_elements failed. adding and linking ghost pad failed !!");
    return FALSE;
  }

  return gst_element_link_many(/*m_audioRate,*/ m_audioConvert, m_rtpL16pay, m_udpSend, NULL);
}

/* HmpLocalAudioSendBin derived class - End
*/
#endif

#if 0
/* AudioVideoMixerChannel construction starts here*/
HmpAudioVideoMixerChannel::HmpAudioVideoMixerChannel(string pipelineName, HmpChannelType pipelineType) :
  HmpChannel(pipelineName, pipelineType), m_pHmpAudioVideoMixerBin(NULL)
{
}

HmpAudioVideoMixerChannel::~HmpAudioVideoMixerChannel()
{
  XGLOG_INFO("HmpMixerChannel::~HmpMixerChannel destruction start");
  if(m_pHmpAudioVideoMixerBin) {
    delete m_pHmpAudioVideoMixerBin;
  }
}

gboolean HmpAudioVideoMixerChannel::construct_bin(string binName, HmpBinType binType)
{
  XGLOG_INFO("HmpAudioVideoMixerChannel::construct_bin %s start",binName.c_str());
  if(!m_pHmpAudioVideoMixerBin) {
    m_pHmpAudioVideoMixerBin = new HmpAudioVideoMixerBin(binName, binType);
    if(!m_pHmpAudioVideoMixerBin->construct()) {
      XGLOG_ERROR( "construct_bin, HmpAudioVideoMixerBin construction failed !!");
      return FALSE;
    } else {
      gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpAudioVideoMixerBin->m_bin), NULL);
    }
  }
  return TRUE;
}

HmpAudioVideoMixerBin::HmpAudioVideoMixerBin(string binName, HmpBinType binType):
  HmpBin(binName, binType), m_webmmux(NULL), m_appsrc(NULL), m_fileSink(NULL), m_video_appsrc(NULL),m_id(NULL)
{

}

HmpAudioVideoMixerBin::~HmpAudioVideoMixerBin()
{
  XGLOG_INFO( "HmpAudioVideoMixerBin::~HmpAudioVideoMixerBin destruction start");
}

gboolean HmpAudioVideoMixerBin::construct()
{
	if(!create_elements()) {
		XGLOG_ERROR( "construct HmpAudioVideoMixerBin, creating elements failed !!");
		return FALSE;
	}

	if(!add_and_link_elements()) {
		XGLOG_ERROR( "construct HmpAudioVideoMixerBin, add and linking elements failed !!");
		return FALSE;
	}
	return TRUE;
}

gboolean HmpAudioVideoMixerBin::create_elements()
{
  XGLOG_INFO( "HmpAudioVideoMixerBin::create_elements start");
  m_appsrc = gst_element_factory_make("appsrc",NULL);
  m_video_appsrc = gst_element_factory_make("appsrc",NULL);
  m_webmmux = gst_element_factory_make("webmmux",NULL);
  m_fileSink = gst_element_factory_make("filesink",NULL);
  m_id = gst_element_factory_make("identity",NULL);
  g_object_set(G_OBJECT(m_id),"signal-handoffs",TRUE,NULL);
  g_signal_connect(G_OBJECT(m_id), "handoff", G_CALLBACK(&user_function),NULL);

  return TRUE;
}

gboolean HmpAudioVideoMixerBin::add_and_link_elements()
{
  GstPad *srcpad,*sinkpad = NULL;
  GstPadLinkReturn lres;

  if(!m_bin || !m_appsrc || !m_webmmux || !m_fileSink)
  {
    XGLOG_ERROR( "construct HmpAudioVideoMixerBin, creating elements failed !!");
  }

  gst_bin_add_many(m_bin, m_appsrc, m_video_appsrc,m_webmmux, m_fileSink, NULL);

  srcpad = gst_element_get_static_pad(m_appsrc,"src");
  sinkpad = gst_element_get_request_pad(m_webmmux,"audio_%u");
  lres = gst_pad_link(srcpad, sinkpad);

  srcpad = NULL;
  sinkpad = NULL; 


  srcpad = gst_element_get_static_pad(m_video_appsrc,"src");
  sinkpad = gst_element_get_request_pad(m_webmmux,"video_%u");
  lres = gst_pad_link(srcpad, sinkpad);


  if(gst_element_link_many(m_webmmux, m_fileSink, NULL) == TRUE)
  {
    XGLOG_INFO( "HmpAudioVideoMixerBin::add_and_link_elements success");
  }else{
    XGLOG_ERROR( "HmpAudioVideoMixerBin::add_and_link_elements success");
  }

  return TRUE;
}

gboolean HmpMixerBin::register_appsink_callback(gpointer data)
{
  XGLOG_ERROR("HmpMixerBin::register_rtpbin_callback !!!!!!");
  Client *client = (Client *)data;
  if(m_appsink){
   g_object_set (G_OBJECT(m_appsink),"emit-signals", TRUE, NULL);
    /* connect the signals to get the data in push mode*/
   g_signal_connect(G_OBJECT (m_appsink), "new-sample",G_CALLBACK (on_new_data_rcvd_appsink), (gpointer)client);
    return TRUE;
  }
  return FALSE;
}

gboolean HmpMixerBin::on_new_data_rcvd_appsink(GstElement *appsink, gpointer data)
{
  
  Client *src_client = (Client *)data;
  //XGLOG_INFO ("HmpMixerBin::on_new_data_rcvd_appsink() !!!!!!!!!!!!!!!!!!!!!!!!%s",src_client->m_detail.m_callId.c_str());
  GstSample *sample = NULL;
  GstBuffer *buffer = NULL;
  GstFlowReturn ret;
  gchar *element_name = NULL;
	element_name = gst_element_get_name(appsink);
  sample = gst_app_sink_pull_sample (GST_APP_SINK (appsink));
  if (!sample){
    XGLOG_ERROR ("Error in pulling sample from appsink element: %s", element_name);
    goto return_from_func;
  }

  GstCaps *caps;
  caps = gst_sample_get_caps(sample);
  // XGLOG_INFO("Got Audio on_new_data_rcvd_appsink Audio Caps are  %s\n",gst_caps_to_string(caps));

  GstElement *AppSrc = src_client->m_pHmpAudioVideoMixerChannel->m_pHmpAudioVideoMixerBin->m_appsrc;
  if(AppSrc){
  // gst_app_src_push_buffer(GST_APP_SRC(AppSrc), buffer);
  g_signal_emit_by_name (AppSrc, "push-sample", sample, &ret);
  }

  return_from_func:
  if (sample){
    gst_sample_unref(sample);
  }

  if (element_name){
    g_free(element_name);
  }

  return GST_FLOW_OK;
}
#endif

HmpMixerBin::HmpMixerBin(string binName, HmpBinType binType):
  HmpBin(binName, binType), m_audioadder(NULL), m_opusenc(NULL), m_appsink(NULL)
{
}

HmpMixerBin::~HmpMixerBin()
{
  XGLOG_INFO( "HmpMixerBin::~HmpMixerBin destruction start");
}

gboolean HmpMixerBin::construct()
{
	if(!create_mix_elements()) {
		XGLOG_ERROR( "construct HmpMixerBin, creating elements failed !!");
		return FALSE;
	}

  if(!add_and_link_mix_elements()){
    XGLOG_ERROR( "construct HmpMixerBin, link elements failed !!");
		return FALSE;
  }
	return TRUE;
}

gboolean HmpMixerBin::create_mix_elements(){
  XGLOG_INFO("HmpMixerBin::create_mix_elements for record bin --->started");
  //we are using adder element for mixing audio streams instead audiomixer
  m_audioadder = gst_element_factory_make("adder", "adder_111");
  m_appsink = gst_element_factory_make("appsink", NULL);
  m_opusenc = gst_element_factory_make("opusenc",NULL);
 
  g_object_set (G_OBJECT(m_opusenc) ,"bitrate", 32000,
                                     "frame-size", 20,
                                     "audio-type", 2048,
                                     "bandwidth", 1103,
                                     "inband-fec", true,
                                     "complexity", 5,
                                     "perfect-timestamp" , true,
                                      NULL);                                    
  return TRUE;
}

gboolean HmpMixerBin::add_and_link_mix_elements(){
  XGLOG_INFO("HmpMixerBin::add_and_link_mix_elements for mixer bin ---> started");
  if(!m_bin || !m_audioadder || !m_appsink ||!m_opusenc ){
    XGLOG_ERROR("Failed to add elements to mixer bin!!!");
  }
  
  gst_bin_add_many(m_bin, m_audioadder, m_opusenc,  m_appsink, NULL);
  
	if(gst_element_link_many(m_audioadder, m_opusenc, m_appsink, NULL) == TRUE){
    XGLOG_INFO("HmpMixerBin::add_and_link_mix_elements for mixer bin ---> success");
	return TRUE;
  }else{
    XGLOG_ERROR("HmpMixerBin::add_and_link_mix_elements for mixer bin failed");
  }

  return TRUE;
}

GstPad *HmpMixerBin::generate_new_audiomixer_Ghostpad()
{
  GstPad *audiomix_sinkpad = NULL;
  GstPadTemplate *audiomix_sinkpad_tmpl = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(m_audioadder),"sink_%u");
  if(audiomix_sinkpad_tmpl) {
     audiomix_sinkpad = gst_element_request_pad(m_audioadder, audiomix_sinkpad_tmpl, NULL, NULL);
     gst_object_unref (GST_OBJECT (audiomix_sinkpad_tmpl));
  }

  XGLOG_INFO( "generate_new_gudiomixer_ghostpad, creating new ghost pad: %p ", audiomix_sinkpad);
		return audiomix_sinkpad;
}

/*MixerChannel/MixerBin construction/handling ends here*/

HmpAudioDecoderBin::HmpAudioDecoderBin(string binName, HmpBinType binType, HmpCodecType codecType):
     HmpBin(binName,binType), m_audioCodecType(codecType), m_opusdec_queue(NULL), m_opusdec(NULL), m_opusdepay(NULL),m_capsfilter(NULL)
{
   XGLOG_INFO( "HmpAudioDecoderBin construction start ");
   m_ulNumAudioPktRcvd = 0;
}

HmpAudioDecoderBin::~HmpAudioDecoderBin(){
   XGLOG_INFO( "HmpAudioDecoderBin::~HmpAudioDecoderBin destruction ended");
}

gboolean HmpAudioDecoderBin::construct()
{
   /*create audio decoder */
   if(!create_decoderbin()) {
      XGLOG_ERROR( "construct AudioDecoderBin, creating failed !!");
      return FALSE;
   }
return TRUE;
}

gboolean HmpAudioDecoderBin::create_decoderbin()
{
   GstPad *pad, *srcpad;
   GstCaps  *caps = NULL;
   XGLOG_INFO( "create audiodecoderBin, creating audiodecoderbin");
   
   m_opusdec = gst_element_factory_make("opusdec", "opusdec_111");
   m_opusdepay = gst_element_factory_make("rtpopusdepay", "opusdepay_111");
   m_opusdec_queue = gst_element_factory_make("queue", "opusdec_queue");
   m_capsfilter = gst_element_factory_make("capsfilter", "capsfilter_opus");
  
   caps = gst_caps_from_string (CAPS_STR_FOR_AUDIODEC);
   g_object_set (G_OBJECT(m_capsfilter) ,"caps", caps, NULL);

   gst_bin_add_many (m_bin, m_opusdepay, m_opusdec_queue, m_opusdec, m_capsfilter, NULL);
   gst_element_link_many(m_opusdec_queue, m_opusdepay, m_opusdec, m_capsfilter, NULL);
  
  XGLOG_INFO( "success create audiodecoderBin, creating audiodecoderbin");
   return TRUE; 
}
