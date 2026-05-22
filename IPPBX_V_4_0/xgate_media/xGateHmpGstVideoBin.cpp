
//local include
#include "xglog.h"

//ace include
#include <ace/Task.h>

#include "xGateMGDispatcher.h"

//self include
#include "xGateHmpGstBin.h"
#include "xGateHmpGstVideoBin.h"

#define THISMODULE "VidBin"

/* HmpVideoBin Base class - Start
 */
/*constructor and destuctor */
HmpVideoBin::HmpVideoBin(string binName, HmpVideoBinType binType) :
  m_binName(binName), m_vbinType(binType), m_bin(NULL),
  m_recvPort(0), m_sendPort(0), m_recvSocket(NULL),
  m_sendSocket(NULL), m_udpRecv(NULL), m_udpSend(NULL)
{
  m_bin = GST_BIN(gst_bin_new(binName.c_str()));
}

HmpVideoBin::~HmpVideoBin()
{
 XGLOG_INFO( "HmpVideoBin::~HmpVideoBin start for bin: %s", m_binName.c_str());
  if(m_recvSocket) {
   XGLOG_INFO( "HmpVideoBin destructing recv port: %d socket", m_recvPort);
    g_socket_close(m_recvSocket, NULL);
    g_object_unref(G_OBJECT(m_recvSocket));
    m_recvSocket = NULL;
    m_recvPort = 0;
  }

  if(m_sendSocket) {
   XGLOG_INFO( "HmpVideoBin destructing send port: %d socket", m_sendPort);
    g_socket_close(m_sendSocket, NULL);
    g_object_unref(G_OBJECT(m_sendSocket));
    m_sendSocket = NULL;
    m_sendPort = 0;
  }
}

gboolean HmpVideoBin::create_udpsrc()
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

      GstCaps* caps = get_video_caps();
      if(caps) {
        g_object_set(G_OBJECT(m_udpRecv), "caps", caps, NULL);
        gst_caps_unref(caps);
      } else {
        XGLOG_ERROR( "create_udpsrc setting video caps for udpsrc failed !!");
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

gboolean HmpVideoBin::create_udpsink()
{
 XGLOG_INFO( "HmpVideoBin::create_udpsink start");
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
    } else {
      XGLOG_ERROR( "create_udpsink, gst_element_make multiudpsink failed !!");
      return FALSE;
    }
  }
  return TRUE;
}

gboolean HmpVideoBin::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
 XGLOG_INFO( "HmpVideoBin::add_client_addr %s:%d start", clientIp.c_str(), clientPort);
  if(m_udpSend) {
    string allClientAddrs("");
    gchar *existingClientAddr;

    char clientAddr[40] = {0};
    snprintf(clientAddr, 40, "%s:%d", clientIp.c_str(), clientPort);
    allClientAddrs.append(clientAddr);

    if(addExistingClients) {
      g_object_get(G_OBJECT(m_udpSend),"clients", &existingClientAddr, NULL);
      if(existingClientAddr[0] != 0) {
        allClientAddrs += ",";
        allClientAddrs.append(existingClientAddr);
      }
      g_free(existingClientAddr);
    }

    g_object_set(G_OBJECT(m_udpSend),"clients", allClientAddrs.c_str(), NULL);
    return TRUE;
  } else {
    return FALSE;
  }
  return TRUE;
}

GstCaps * HmpVideoBin::get_video_caps()
{
  GstCaps *caps = NULL;
  switch (m_vbinType) {
    case EN_BIN_VID_VP8_RECV:
      {
       XGLOG_INFO( "HmpVideoBin::get_caps for VID_VP8_RECV bin from UDP_CAPS_VID_VP8 string");
        caps = gst_caps_from_string(UDP_CAPS_VID_VP8);
        break;
      }
    case EN_BIN_VID_VP9_RECV:
      {
       XGLOG_INFO( "HmpVideoBin::get_caps for VID_VP9_RECV bin from UDP_CAPS_VID_VP9 string");
        caps = gst_caps_from_string(UDP_CAPS_VID_VP9);
        break;
      }
    case EN_BIN_VID_H264_RECV:
      {
       XGLOG_INFO( "HmpVideoBin::get_caps for VID_H264_RECV bin from UDP_CAPS_VID_H264 string");
        caps = gst_caps_from_string(UDP_CAPS_VID_H264);
        break;
      }
    case EN_BIN_VID_H265_RECV:
      {
       XGLOG_INFO( "HmpVideoBin::get_caps for VID_H265_RECV bin from UDP_CAPS_VID_H265 string");
        caps = gst_caps_from_string(UDP_CAPS_VID_H265);
        break;
      }
    case EN_BIN_VID_DTLS_RECV:
      {
       XGLOG_INFO( "HmpVideoBin::get_caps for DTLS_RECV bin from UDP_CAPS_VID_DTLS string");
        caps = gst_caps_from_string(UDP_CAPS_DTLS);
        break;
      }

    case EN_BIN_VID_DTLS_SEND:
      {
       XGLOG_INFO( "HmpVideoBin::get_caps for DTSL_SEND bin from UDP_CAPS_VID_DTLS_SEND string");
        caps = gst_caps_from_string(UDP_CAPS_DTLS);
        break;
      }

    default:
      {
       XGLOG_INFO( "HmpVideoBin::get_caps failed for Unknown/Unsupported bin");
        break;
      }
  }
  return caps;
}

gboolean HmpVideoBin::draw_dot_file()
{
  if(GST_DEBUG && m_bin && !m_binName.empty()) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_bin, GST_DEBUG_GRAPH_SHOW_ALL, m_binName.c_str());
    return TRUE;
  }
  return FALSE;
}
/* HmpVideoBin Base class - End
 */


/* HmpFrameRecvBin derived class - start 
*/
HmpFrameRecvBin::HmpFrameRecvBin(string binName, HmpVideoBinType binType, gint recvPort, GSocket *recvSock) :
  HmpVideoBin(binName, binType), m_dtlssrtpDec(NULL), m_rtpBin(NULL)
{
 XGLOG_INFO( "HmpFrameRecvBin::HmpFrameRecvBin construction start");
  m_recvPort = recvPort;
  m_recvSocket = recvSock;
}

HmpFrameRecvBin::~HmpFrameRecvBin()
{
 XGLOG_INFO( "HmpFrameRecvBin::~HmpFrameRecvBin destruction start");
}

gboolean HmpFrameRecvBin::construct()
{
 XGLOG_INFO( "HmpFrameRecvBin::construct start");

  if(!create_udpsrc()) {
    XGLOG_ERROR( "construct FrameRecvBin, creating udpsrc failed !!");
    return FALSE;
  }

  /*allocate loopback port and updsink for internal packet routing */
  while(!m_sendSocket) {
    m_sendPort = HOSTGST_GET_VIDEO_LOOPBACK_PORT();
    m_sendSocket = xGateHmpGstUtil::create_udpsocket(m_sendPort);
  }
  if(!create_udpsink()) {
    XGLOG_ERROR( "construct FrameRecvBin, creating udpsink failed !!");
    return FALSE;
  }

  /*create rtpbin element */
  /*if(!create_rtpbin()) {
    XGLOG_ERROR( "construct FrameRecvBin, creating rtpbin failed !!");
    return FALSE;
  }
  */
  /*create required elements */
  if(!create_elements()) {
    XGLOG_ERROR( "construct FrameRecvBin, creating elements failed !!");
    return FALSE;
  }

  /* add and link elements in bin container */
  if(!add_and_link_elements()) {
    XGLOG_ERROR( "construct FrameRecvBin, add and linking elements failed !!");
    return FALSE;
  }

  return TRUE;
}


gboolean HmpFrameRecvBin::create_rtpbin()
{
 XGLOG_INFO( "HmpFrameRecvBin::create_rtpbin start");
  guint latency = 40;
  m_rtpBin = gst_element_factory_make("rtpbin", NULL);
  if(!m_rtpBin) {
    XGLOG_ERROR( "HmpFrameRecvBin::create_rtpbin gst_element_make rtpbin failed !!");
    return FALSE;
  }

  g_object_set(G_OBJECT(m_rtpBin),"autoremove", TRUE, NULL);
  g_object_set(G_OBJECT(m_rtpBin), "latency", latency, NULL);
  return TRUE;
}

gboolean HmpFrameRecvBin::register_rtpbin_callback(gpointer data)
{
  if(m_rtpBin) {
    //g_signal_connect(G_OBJECT(m_rtpBin), "pad-added", G_CALLBACK(&on_pad_added), data);
    /*g_signal_connect(G_OBJECT(m_rtpBin), "pad-removed", G_CALLBACK(&on_pad_removed), data); */
    //g_signal_connect(G_OBJECT(m_rtpBin), "request-pt-map", G_CALLBACK(&on_request_pt_map), data);
    return TRUE;
  }
  return FALSE;
}


gboolean HmpFrameRecvBin::set_stundtlssrtpdec_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId)
{
  if(m_dtlssrtpDec) {
    g_object_set(G_OBJECT(m_dtlssrtpDec), "connection-id", stundtlsConnId.c_str(), NULL);

    gint sockfd = 0;
    sockfd = g_socket_get_fd(m_recvSocket);
    if(sockfd) {
      g_object_set(G_OBJECT(m_dtlssrtpDec), "sock-des", sockfd, NULL);
    } else {
      XGLOG_ERROR( "set_stundtlssrtpdec_attributes failed to set sockfd !!");
      return FALSE;
    }

    if(!secureDetail.m_candidateDetail.m_raddr.empty()) {
      g_object_set(G_OBJECT(m_dtlssrtpDec), "stun-host", secureDetail.m_candidateDetail.m_raddr.c_str(), NULL);
    } else {
      XGLOG_ERROR( "set_stundtlssrtpdec_attributes failed, remote candidate address is empty !!");
      return FALSE;
    }

    if(secureDetail.m_candidateDetail.m_rport > 0) {
      g_object_set(G_OBJECT(m_dtlssrtpDec), "stun-port", secureDetail.m_candidateDetail.m_rport, NULL);
    } else {
      XGLOG_ERROR( "set_stundtlssrtpdec_attributes failed, remote candidate port is invalid !!");
      return FALSE;
    }

    if(!secureDetail.m_rIceUfrag.empty() && !secureDetail.m_lIceUfrag.empty()) {
      char iceUfrag[50] = {0};
      sprintf(iceUfrag, "%s:%s", secureDetail.m_lIceUfrag.c_str(), secureDetail.m_rIceUfrag.c_str());
      g_object_set(G_OBJECT(m_dtlssrtpDec), "stun-uname", iceUfrag, NULL);
    } else {
      XGLOG_ERROR( "set_stundtlssrtpdec_attributes failed, remote/local stun IceUfrag is empty !!");
      return FALSE;
    }

    if(!secureDetail.m_lIcePwd.empty()) {
      g_object_set(G_OBJECT(m_dtlssrtpDec), "stun-upass", secureDetail.m_lIcePwd.c_str(), NULL);
    } else {
      XGLOG_ERROR( "set_stundtlssrtpdec_attributes failed, local stun IcePwd is empty !!");
      return FALSE;
    }

    if(!secureDetail.m_pemFile.empty()) {
      g_object_set(G_OBJECT(m_dtlssrtpDec), "pem", secureDetail.m_pemFile.c_str(), NULL);
    } else {
      XGLOG_ERROR( "set_stundtlssrtpdec_attributes failed, pem file path is empty !!");
      return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

gboolean HmpFrameRecvBin::create_elements()
{
 XGLOG_INFO( "HmpFrameRecvBin::create_elements start");
  switch(m_vbinType) {
    case EN_BIN_VID_DTLS_RECV:
      {
       XGLOG_INFO( "create_elements for DTLS_RECV bin");
        m_dtlssrtpDec = gst_element_factory_make("stundtlssrtpdec", NULL);
        break;
      }
    default:
      {
        XGLOG_ERROR( "create_elements failed for Unknown/Unsupported frame recv bin !!");
        return FALSE;
      }
  }
  return TRUE;
}

gboolean HmpFrameRecvBin::add_and_link_elements()
{
 XGLOG_INFO( "HmpFrameRecvBin::add_and_link_elements start");

  switch(m_vbinType) {

    case EN_BIN_VID_DTLS_RECV:
      {
       XGLOG_INFO( "add_and_link_elements for FRAME DTLS_RECV bin");
        if(!m_dtlssrtpDec) {
          XGLOG_ERROR( "add_and_link_elements failed. elements not creaed properly DTLS_RECV bin !!");
          return FALSE;
        } else {
          gst_bin_add_many(m_bin, m_udpRecv, m_dtlssrtpDec, m_rtpBin, m_udpSend, NULL);
          gst_element_link_many(m_udpRecv, m_dtlssrtpDec, m_udpSend, NULL);
        }
        break;
      }

    default:
      {
       XGLOG_INFO( "add_and_link_elements failed for Unknown/Unsupported FRAME recv bin");
        break;
      }
  }
  return TRUE;
}
/* HmpFrameRecvBin derived class - End
 */

/* HmpFrameSendBin derived class - Start
 */
/*constructor and destuctor */
HmpFrameSendBin::HmpFrameSendBin(string binName, HmpVideoBinType binType, GSocket *sendSocket) :
  HmpVideoBin(binName, binType), m_dtlssrtpEnc(NULL)
{
  m_sendSocket = sendSocket;
 XGLOG_INFO( "HmpFrameSendBin::HmpFrameSendBin construction start");
}

HmpFrameSendBin::~HmpFrameSendBin()
{
 XGLOG_INFO( "HmpFrameSendBin::~HmpFrameSendBin destruction start");
#if 1 /*TODO: */
  /*Note: We are purposely making it NULL to avoid crash. */
  /*it's not recommented and this will make socket open */
  m_sendSocket = NULL;
#endif
}

gboolean HmpFrameSendBin::construct()
{
 XGLOG_INFO( "HmpFrameSendBin::construct start");
  /*allocate loopback port and create udpsrc to receive internal packets */
  while(!m_recvSocket) {
    m_recvPort = HOSTGST_GET_VIDEO_LOOPBACK_PORT();
    m_recvSocket = xGateHmpGstUtil::create_udpsocket(m_recvPort);
  }

  if(!create_udpsrc()) {
    XGLOG_ERROR( "construct FrameSendBin, creating udpsrc failed !!");
    return FALSE;
  }

  /*create udpsink to send packets to client */
  if(!create_udpsink()) {
    XGLOG_ERROR( "construct FrameSendBin, creating udpsink failed !!");
    return FALSE;
  }

  /*create required encoded elements  */
  if(!create_elements()) {
    XGLOG_ERROR( "construct FrameSendBin, creating elements failed !!");
    return FALSE;
  }

  /*add and link elements in bin container */
  if(!add_and_link_elements()) {
    XGLOG_ERROR( "construct FrameSendBin, add and linking elements failed !!");
    return FALSE;
  }

  return TRUE;
}

gboolean HmpFrameSendBin::create_elements()
{
 XGLOG_INFO( "HmpFrameSendBin::create_elements start");
  switch(m_vbinType) {
 
    case EN_BIN_VID_DTLS_SEND:
      {
       XGLOG_INFO( "create_encode_elements for FRAME DTLS_SEND bin");
        m_dtlssrtpEnc = gst_element_factory_make("stundtlssrtpenc", NULL);
        break;
      }

    default:
      {
        XGLOG_ERROR( "create_encode_elements failed for Unknown/Unsupported Frame send bin !!");
        return FALSE;
      }
  }

  return TRUE;
}

gboolean HmpFrameSendBin::add_and_link_elements()
{
 XGLOG_INFO( "HmpFrameSendBin::add_and_link_elements start");
  switch(m_vbinType) {

    case EN_BIN_VID_DTLS_SEND:
      {
        if(!m_dtlssrtpEnc) {
          XGLOG_ERROR( "add_and_link_elements failed. elements not creaed properly for DTLS_SEND bin !!");
          return FALSE;
        } else {
         XGLOG_INFO( "add_and_link_elements for DTLS_SEND bin");
          gst_bin_add_many(m_bin, m_udpRecv, m_dtlssrtpEnc, m_udpSend, NULL);
          gst_element_link_many(m_udpRecv, m_dtlssrtpEnc, m_udpSend, NULL);
        }
        break;
      }
    default:
      {
       XGLOG_INFO( "add_and_link_elements failed for Unknown/Unsupported Frame recv bin");
        break;
      }
  }
  return TRUE;
}

gboolean HmpFrameSendBin::set_stundtlssrtpenc_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId)
{
  if(m_dtlssrtpEnc) {
    if(!stundtlsConnId.empty()) {
      g_object_set (G_OBJECT (m_dtlssrtpEnc),"connection-id", stundtlsConnId.c_str(), NULL);
    } else {
      XGLOG_ERROR( "set_stundtlssrtpenc_attributes failed. stundtlsConnId info is empty !!");
      return FALSE;
    }
    g_object_set (G_OBJECT (m_dtlssrtpEnc), "is-client", FALSE, NULL);
    return TRUE;
  }
  return FALSE;
}
/* HmpFrameSendBin derived class - End  
*/

/* HmpFrameDecoderBin derived class - Start
 * */
HmpFrameDecoderBin::HmpFrameDecoderBin(string binName, HmpVideoBinType binType) :
  HmpVideoBin(binName, binType),  m_videoRtpDepay(NULL), m_videoDecoder(NULL), m_videoConvert(NULL)
{
}

HmpFrameDecoderBin::~HmpFrameDecoderBin()
{
}

gboolean HmpFrameDecoderBin::construct()
{
 XGLOG_INFO( "HmpFrameDecoderBin::construct start");

  /*allocate loopback port and create udpsrc to receive internal packets */
  while(!m_recvSocket) {
    m_recvPort = HOSTGST_GET_VIDEO_LOOPBACK_PORT();
    m_recvSocket = xGateHmpGstUtil::create_udpsocket(m_recvPort);
  }

  if(!create_udpsrc()) {
    XGLOG_ERROR( "construct HmpFrameDecoderBin, creating udpsrc failed !!");
    return FALSE;
  }

  /*create required decode elements */
  if(!create_decode_elements()) {
    XGLOG_ERROR( "construct HmpFrameDecoderBin, creating decode elements failed !!");
    return FALSE;
  }

  /*add and link elements in bin container */
  if(!add_and_link_elements()) {
    XGLOG_ERROR( "construct HmpFrameDecoderBin, add and linking elements failed !!");
    return FALSE;
  }

  return TRUE;
}

gboolean HmpFrameDecoderBin::create_decode_elements()
{
 XGLOG_INFO( "HmpFrameDecoderBin::create_decode_elements start");
  guint latency = 40;
  switch(m_vbinType) {

    case EN_BIN_VID_VP8_RECV:
      {
       XGLOG_INFO( "create_decode_elements for VID_VP8_RECV bin");
        m_videoRtpDepay = gst_element_factory_make("rtpvp8depay", NULL);
        m_videoDecoder = gst_element_factory_make("vp8dec", NULL);
        break;
      }

    case EN_BIN_VID_VP9_RECV:
      {
       XGLOG_INFO( "create_decode_elements for VID_VP8_RECV bin");
        m_videoRtpDepay = gst_element_factory_make("rtpvp9depay", NULL);
        m_videoDecoder = gst_element_factory_make("vp9dec", NULL);
        break;
      }

    case EN_BIN_VID_H264_RECV:
      {
       XGLOG_INFO( "create_decode_elements for VID_H264_RECV bin");
        m_videoRtpDepay = gst_element_factory_make("rtph264depay", NULL);
        m_videoDecoder = gst_element_factory_make("openh264dec", NULL);
        break;
      }

    case EN_BIN_VID_H265_RECV:
      {
       XGLOG_INFO( "create_decode_elements for VID_H265_RECV bin");
        m_videoRtpDepay = gst_element_factory_make("rtph265depay", NULL);
        m_videoDecoder = gst_element_factory_make("libde265dec ", NULL);
        break;
      }

    default:
      {
        XGLOG_ERROR( "create_decode_elements failed for Unknown/Unsupported frame recv bin !!");
        return FALSE;
      }
  }

  m_videoConvert = gst_element_factory_make("videoconvert", NULL);

  return TRUE;
}

gboolean HmpFrameDecoderBin::add_and_link_elements()
{
 XGLOG_INFO( "HmpFrameDecoderBin::add_and_link_elements start");
  if(!m_videoRtpDepay || !m_videoDecoder || !m_videoConvert) {
    XGLOG_ERROR( "add_and_link_elements failed. elements not creaed properly !!");
    return FALSE;
  } else {
    gst_bin_add_many(m_bin, m_udpRecv, m_videoRtpDepay, m_videoDecoder, m_videoConvert, NULL);
    gst_element_link_many(m_udpRecv, m_videoRtpDepay, m_videoDecoder, m_videoConvert, NULL);
  }

  GstPad *pad = gst_element_get_static_pad(m_videoConvert, "src");
  if(pad) {
    m_ghostRtpSrcPad = gst_ghost_pad_new("src", pad);
    gst_element_add_pad(GST_ELEMENT(m_bin), m_ghostRtpSrcPad);
    gst_object_unref(pad);
  }
  return TRUE;
}
/* HmpFrameDecoderBin derived class - End
 */


/* HmpFrameEncoderBin derived class - Start
 * */
HmpFrameEncoderBin::HmpFrameEncoderBin(string binName, HmpVideoBinType binType) :
  HmpVideoBin(binName, binType), m_videoScale(NULL), m_videoRate(NULL), m_videoEncoder(NULL), m_videoRtpPay(NULL)
{
}

HmpFrameEncoderBin::~HmpFrameEncoderBin()
{
}

gboolean HmpFrameEncoderBin::construct()
{
  /*allocate loopback port and updsink for internal packet routing */
  while(!m_sendSocket) {
    m_sendPort = HOSTGST_GET_VIDEO_LOOPBACK_PORT();
    m_sendSocket = xGateHmpGstUtil::create_udpsocket(m_sendPort);
  }
  if(!create_udpsink()) {
    XGLOG_ERROR( "construct HmpFrameEncoderBin, creating udpsink failed !!");
    return FALSE;
  }

  /*create required encoded elements */
  if(!create_encode_elements()) {
    XGLOG_ERROR( "construct HmpFrameEncoderBin, creating encode elements failed !!");
    return FALSE;
  }

  /*add and link elements in bin container */
  if(!add_and_link_elements()) {
    XGLOG_ERROR( "construct HmpFrameEncoderBin, add and linking elements failed !!");
    return FALSE;
  }
  return TRUE;
}


gboolean HmpFrameEncoderBin::create_encode_elements()
{
 XGLOG_INFO( "HmpFrameEncoderBin::create_encode_elements start");
  switch(m_vbinType) {
    case EN_BIN_VID_VP8_SEND:
      {
       XGLOG_INFO( "create_encode_elements for VID_VP8_SEND bin");
        m_videoEncoder = gst_element_factory_make("vp8enc", NULL);
        m_videoRtpPay = gst_element_factory_make("rtpvp8pay", NULL);

        g_object_set(G_OBJECT(m_videoRtpPay),"pt", 125, NULL);
        break;
      }
    case EN_BIN_VID_VP9_SEND:
      {
       XGLOG_INFO( "create_encode_elements for VID_VP9_SEND bin");
        m_videoEncoder = gst_element_factory_make("vp9enc", NULL);
        m_videoRtpPay = gst_element_factory_make("rtpvp9pay", NULL);

        g_object_set(G_OBJECT(m_videoRtpPay),"pt", 127, NULL);
        break;
      }
    case EN_BIN_VID_H264_SEND:
      {
       XGLOG_INFO( "create_encode_elements for VID_H264_SEN bin");
        m_videoEncoder = gst_element_factory_make("openh264enc", NULL);
        m_videoRtpPay = gst_element_factory_make("rtph264pay", NULL);

        g_object_set(G_OBJECT(m_videoRtpPay),"pt", 121, NULL);
        break;
      }
    case EN_BIN_VID_H265_SEND:
      {
       XGLOG_INFO( "create_encode_elements for G729_SEND bin");
        m_videoEncoder = gst_element_factory_make("x265enc", NULL);
        m_videoRtpPay = gst_element_factory_make("rtph265pay", NULL);

        g_object_set(G_OBJECT(m_videoRtpPay),"pt", 123, NULL);
        break;
      }
    default:
      {
        XGLOG_ERROR( "create_encode_elements failed for Unknown/Unsupported voice send bin !!");
        return FALSE;
      }
  }

  m_videoScale = gst_element_factory_make("videoscale", NULL);
  m_videoRate = gst_element_factory_make("videorate", NULL);

  return TRUE;
}

gboolean HmpFrameEncoderBin::add_and_link_elements()
{
 XGLOG_INFO( "HmpFrameEncoderBin::add_and_link_elements start");
  if(!m_videoScale || !m_videoRate || !m_videoEncoder || !m_videoRtpPay) {
    XGLOG_ERROR( "add_and_link_elements failed. elements not creaed properly for VID_SEND bin !!");
    return FALSE;
  } else {
   XGLOG_INFO( "add_and_link_elements for VID_SEND bin");

    gst_bin_add_many(m_bin, m_videoScale, m_videoRate, m_videoEncoder, m_videoRtpPay, m_udpSend, NULL);
    gst_element_link_many(m_videoScale, m_videoRate, m_videoEncoder, m_videoRtpPay, m_udpSend, NULL);
  }

  GstPad *pad = gst_element_get_static_pad (m_videoScale, "sink");
  if(pad) {
    m_ghostRtpSinkPad = gst_ghost_pad_new("sink", pad);
    gst_element_add_pad (GST_ELEMENT(m_bin), m_ghostRtpSinkPad);
    gst_pad_set_active(m_ghostRtpSinkPad, TRUE);
    gst_object_unref (GST_OBJECT (pad));
  }

  return TRUE;
}

/* HmpFrameEncoderBin derived class - End
 */

/* HmpPlayFrameBin derived class - Start
 */
HmpPlayFrameBin::HmpPlayFrameBin(string binName, HmpVideoBinType binType) :
  HmpVideoBin(binName, binType), m_fileSrc(NULL), m_rawVideoParse(NULL),
  m_videoConvert(NULL), m_videoScale(NULL), m_videoEncoder(NULL),
  m_videoRtpPay(NULL)
{
}

HmpPlayFrameBin::~HmpPlayFrameBin()
{
}

gboolean HmpPlayFrameBin::construct()
{
  //allocate loopback port and updsink for internal packet routing
  while(!m_sendSocket) {
    m_sendPort = HOSTGST_GET_LOOPBACK_PORT();
    m_sendSocket = xGateHmpGstUtil::create_udpsocket(m_sendPort);
  }

  if(!create_udpsink()) {
    XGLOG_ERROR( "construct HmpPlayframeBin, creating udpsink failed !!");
    return FALSE;
  }
  /*create required encoded elements */
  if(!create_encode_elements()) {
    XGLOG_ERROR( "construct HmpPlayFrameBin, creating encode elements failed !!");
    return FALSE;
  }

  /*add and link elements in bin container */
  if(!add_and_link_elements()) {
    XGLOG_ERROR( "construct HmpPlayFrameBin, add and linking elements failed !!");
    return FALSE;
  }
  return TRUE;
}
 
gboolean HmpPlayFrameBin::create_encode_elements()
{
 XGLOG_INFO( "HmpPlayFrameBin::create_encode_elements start");
  //create required elements
  m_fileSrc = gst_element_factory_make ("filesrc", NULL);
  m_rawVideoParse = gst_element_factory_make ("rawvideoparse", NULL);

  switch(m_vbinType) {

    case EN_BIN_VID_VP8_SEND:
      {
       XGLOG_INFO( "create_encode_elements for VID_VP8_PLAY bin");
        m_videoEncoder = gst_element_factory_make("vp8enc", NULL);
        m_videoRtpPay = gst_element_factory_make("rtpvp8pay", NULL);

        g_object_set(G_OBJECT(m_videoRtpPay),"pt", 125, NULL);
        break;
      }
    case EN_BIN_VID_VP9_SEND:
      {
       XGLOG_INFO( "create_encode_elements for VID_VP9_PLAY bin");
        m_videoEncoder = gst_element_factory_make("vp9enc", NULL);
        m_videoRtpPay = gst_element_factory_make("rtpvp9pay", NULL);

        g_object_set(G_OBJECT(m_videoRtpPay),"pt", 127, NULL);
        break;
      }
    case EN_BIN_VID_H264_SEND:
      {
       XGLOG_INFO( "create_encode_elements for VID_H264_PLAY bin");
        m_videoEncoder = gst_element_factory_make("openh264enc", NULL);
        m_videoRtpPay = gst_element_factory_make("rtph264pay", NULL);

        g_object_set(G_OBJECT(m_videoRtpPay),"pt", 121, NULL);
        break;
      }
    case EN_BIN_VID_H265_SEND:
      {
       XGLOG_INFO( "create_encode_elements for G729_PLAY bin");
        m_videoEncoder = gst_element_factory_make("x265enc", NULL);
        m_videoRtpPay = gst_element_factory_make("rtph265pay", NULL);

        g_object_set(G_OBJECT(m_videoRtpPay),"pt", 123, NULL);
        break;
      }
    default:
      {
        XGLOG_ERROR( "create_encode_elements failed for Unknown/Unsupported voice send bin !!");
        return FALSE;
      }
  }
  m_videoConvert = gst_element_factory_make("videoconvert", NULL);
  m_videoScale = gst_element_factory_make("videoscale", NULL);

  return TRUE;
}

gboolean HmpPlayFrameBin::add_and_link_elements()
{
 XGLOG_INFO( "HmpPlayFrameBin::add_and_link_elements start");
  if(!m_videoConvert || !m_videoScale || !m_videoEncoder || !m_videoRtpPay) {
    XGLOG_ERROR( "add_and_link_elements failed. elements not creaed properly for VID_PLAY bin !!");
    return FALSE;
  } else {
   XGLOG_INFO( "add_and_link_elements for VID_PLAY bin");

    gst_bin_add_many(m_bin, m_fileSrc, m_rawVideoParse, m_videoConvert, m_videoScale, m_videoEncoder, m_videoRtpPay, m_udpSend, NULL);
    gst_element_link_many(m_fileSrc, m_rawVideoParse, m_videoConvert, m_videoScale, m_videoEncoder, m_videoRtpPay, m_udpSend, NULL);
  }

  return TRUE;
}

gboolean HmpPlayFrameBin::set_play_file(string playFile)
{
 XGLOG_INFO( "HmpPlayFrameBin::set_play_file %s start", playFile.c_str());
  if(m_fileSrc) {
    g_object_set(G_OBJECT(m_fileSrc),"location", playFile.c_str(), NULL);
    return TRUE;
  }
  return FALSE;
}
/* HmpFrameEncoderBin derived class - End
 */

/* HmpVideoChannel bass class - Start
 */
HmpVideoChannel::HmpVideoChannel(string pipelineName, HmpChannelType pipelineType) :
  m_pipelineName(pipelineName), m_pipelineType(pipelineType), m_pipeline(NULL),
  m_bus(NULL), m_busId(0)
{
 XGLOG_INFO( "HmpVideoChannel::HmpVideoChannel construction start");
  m_pipeline = gst_pipeline_new(m_pipelineName.c_str());
  if(!m_pipeline) {
    XGLOG_ERROR( "HmpVideoChannel::HmpVideoChannel, %s pipeline object creation failed!");
    return;
  }

  m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
}

HmpVideoChannel::~HmpVideoChannel()
{
 XGLOG_INFO( "HmpVideoChannel::~HmpVideoChannel destructing %s start", m_pipelineName.c_str());
  if(m_pipeline) {
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (m_pipeline));
  }

  if(m_bus) {
    gst_bus_set_sync_handler(m_bus, NULL, NULL, NULL);
    gst_object_unref(GST_OBJECT (m_bus));
  }

  if(m_busId) {
    g_source_remove (m_busId);
  }
}

gboolean HmpVideoChannel::register_handle_event_callback(gpointer data)
{
  if(m_pipeline && m_bus && data) {
    m_busId = gst_bus_add_watch(m_bus, &handle_event, data);
    return (m_busId != nullptr);
  }
  return FALSE;
}

gboolean HmpVideoChannel::handle_event(GstBus* bus, GstMessage* msg, gpointer data)
{
  Client *client = (Client *)data;
  if(!client) {
    XGLOG_ERROR( "HmpVideoChannel::handle_event, invalid client object !!");
    return TRUE;
  }

  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      {
       XGLOG_INFO( "HmpVideoChannel::handle_event got GST_MESSAGE_ERROR message");
        GError *err = NULL;
        gchar *dbgInfo = NULL;
        if(GST_DEBUG) {
          gst_message_parse_error(msg, &err, &dbgInfo);
          if(err) {
            g_printerr("ERROR from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
            XGLOG_ERROR( "HmpVideoChannel::handle_event ERROR from element %s: %s", GST_OBJECT_NAME(msg->src), err->message);
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
       XGLOG_INFO( "HmpVideoChannel::handle_event got GST_MESSAGE_UNKNOWN/DEFAULT message");
        break;
      }
  }
  return TRUE; /*it's important to return TRUE from this function */
}

gboolean HmpVideoChannel::draw_dot_file()
{
  gboolean retVal = FALSE;
  if(GST_DEBUG && m_pipeline && !m_pipelineName.empty()) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_pipelineName.c_str());
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
  if(m_pipeline) {
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    retVal = TRUE;
  }
  draw_dot_file();
  return retVal;
}
/* HmpVideoChannel bass class - End
 */


/* HmpFrameRecvChannel derived class - Start
 */
HmpFrameRecvChannel::HmpFrameRecvChannel(string pipelineName, HmpChannelType pipelineType) :
  HmpVideoChannel(pipelineName, pipelineType) , m_pHmpFrameRecvBin(NULL)
{
 XGLOG_INFO( "HmpFrameRecvChannel::HmpFrameRecvChannel construction start");
}

HmpFrameRecvChannel::~HmpFrameRecvChannel()
{
 XGLOG_INFO( "HmpFrameRecvChannel::~HmpFrameRecvChannel destruction start");
  if(m_pHmpFrameRecvBin) {
    delete m_pHmpFrameRecvBin;
  }
}

gboolean HmpFrameRecvChannel::construct_bin(string binName, HmpVideoBinType vbinType, gint recvPort, GSocket *recvSock)
{
 XGLOG_INFO( "HmpFrameRecvChannel::construct_bin %s start", binName.c_str());
  if(!m_pHmpFrameRecvBin) {
    m_pHmpFrameRecvBin = new HmpFrameRecvBin(binName, vbinType, recvPort, recvSock);
    if(!m_pHmpFrameRecvBin->construct()) {
      XGLOG_ERROR( "construct_bin, FrameRecvBin construction failed !!");
      return FALSE;
    } else {
      gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpFrameRecvBin->m_bin), NULL);
    }
  }
  return TRUE;
}

GSocket * HmpFrameRecvChannel::get_recv_socket()
{
  GSocket *socket = NULL;
  if(m_pHmpFrameRecvBin) {
    socket = m_pHmpFrameRecvBin->m_recvSocket;
  }
  return socket;
}

void HmpFrameRecvChannel::set_recv_socket(GSocket *socket)
{
  if(m_pHmpFrameRecvBin) {
    m_pHmpFrameRecvBin->m_recvSocket = socket;
  }
}

gint HmpFrameRecvChannel::get_recv_port()
{
  gint recvPort = 0;
  if(m_pHmpFrameRecvBin) {
    recvPort = m_pHmpFrameRecvBin->m_recvPort;
  }
  return recvPort;
}

void HmpFrameRecvChannel::set_recv_port(gint port)
{
  if(m_pHmpFrameRecvBin) {
    m_pHmpFrameRecvBin->m_recvPort = port;
  }
}
gboolean HmpFrameRecvChannel::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
  if(m_pHmpFrameRecvBin) {
    m_pHmpFrameRecvBin->add_client_addr(clientIp, clientPort, addExistingClients);
    return TRUE;
  }
  return FALSE;
}

gboolean HmpFrameRecvChannel::register_rtpbin_callback(gpointer data)
{
  if(m_pHmpFrameRecvBin) {
    return m_pHmpFrameRecvBin->register_rtpbin_callback(data);
  }
  return FALSE;
}

gboolean HmpFrameRecvChannel::set_stundtlssrtpdec_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId)
{
  if(m_pHmpFrameRecvBin) {
    return m_pHmpFrameRecvBin->set_stundtlssrtpdec_attributes(secureDetail, stundtlsConnId);
  }
  return FALSE;
}
/*  HmpFrameRecvChannel derived class - End
 */


/* HmpFrameSendChannel derived class - Start
 */
HmpFrameSendChannel::HmpFrameSendChannel(string pipelineName, HmpChannelType pipelineType) :
  HmpVideoChannel(pipelineName, pipelineType) , m_pHmpFrameSendBin(NULL)
{
 XGLOG_INFO( "HmpFrameSendChannel::HmpFrameSendChannel construction start");
}

HmpFrameSendChannel::~HmpFrameSendChannel()
{
 XGLOG_INFO( "HmpFrameSendChannel::~HmpFrameSendChannel destruction start");
  if(m_pHmpFrameSendBin) {
    delete m_pHmpFrameSendBin;
  }
}

gboolean HmpFrameSendChannel::construct_bin(string binName, HmpVideoBinType vbinType, GSocket *sendSocket)
{
 XGLOG_INFO( "HmpFrameSendChannel::construct_bin %s start", binName.c_str());
  if(!m_pHmpFrameSendBin) {
    m_pHmpFrameSendBin = new HmpFrameSendBin(binName, vbinType, sendSocket);
    if(!m_pHmpFrameSendBin->construct()) {
      XGLOG_ERROR( "construct_bin, FrameSendBin construction failed !!");
      return FALSE;
    } else {
      gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpFrameSendBin->m_bin), NULL);
    }
  }
  return TRUE;
}

gint HmpFrameSendChannel::get_recv_port()
{
  gint recvPort = 0;
  if(m_pHmpFrameSendBin) {
    recvPort = m_pHmpFrameSendBin->m_recvPort;
  }
  return recvPort;
}

gboolean HmpFrameSendChannel::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
  if(m_pHmpFrameSendBin) {
    m_pHmpFrameSendBin->add_client_addr(clientIp, clientPort, addExistingClients);
    m_pHmpFrameSendBin->m_sendIp = clientIp;
    m_pHmpFrameSendBin->m_sendPort = clientPort;
    return TRUE;
  }
  return FALSE;
}

gboolean HmpFrameSendChannel::set_stundtlssrtpenc_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId)
{
  if(m_pHmpFrameSendBin) {
    return m_pHmpFrameSendBin->set_stundtlssrtpenc_attributes(secureDetail, stundtlsConnId);
  }
  return FALSE;
}
/*  HmpFrameSendChannel derived class - End
 */


/*  HmpFrameDecoderChannel derived class - Start
 */
HmpFrameDecoderChannel::HmpFrameDecoderChannel(string pipelineName, HmpChannelType pipelineType) :
  HmpVideoChannel(pipelineName, pipelineType) , m_pHmpFrameDecoderBin(NULL)
{
 XGLOG_INFO( "HmpFrameDecoderChannel::HmpFrameDecoderChannel construction start");
}

HmpFrameDecoderChannel::~HmpFrameDecoderChannel()
{
 XGLOG_INFO( "HmpframeDecoderChannel::~HmpFrameDecoderChannel destruction start");
  if(m_pHmpFrameDecoderBin) {
    delete m_pHmpFrameDecoderBin;
  }
}

gboolean HmpFrameDecoderChannel::construct_bin(string binName, HmpVideoBinType binType)
{
 XGLOG_INFO( "HmpFrameDecoderChannel::construct_bin %s start", binName.c_str());
  if(!m_pHmpFrameDecoderBin) {
    m_pHmpFrameDecoderBin = new HmpFrameDecoderBin(binName, binType);
    if(!m_pHmpFrameDecoderBin->construct()) {
      XGLOG_ERROR( "construct_bin, HmpframeDecoderBin construction failed !!");
      return FALSE;
    } else {
      gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpFrameDecoderBin->m_bin), NULL);
    }
  }
  return TRUE;
}

gboolean HmpFrameDecoderChannel::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
  if(m_pHmpFrameDecoderBin) {
    m_pHmpFrameDecoderBin->add_client_addr(clientIp, clientPort, addExistingClients);
    return TRUE;
  }
  return FALSE;
}

gint HmpFrameDecoderChannel::get_recv_port()
{
  gint recvPort = 0;
  if(m_pHmpFrameDecoderBin) {
    recvPort = m_pHmpFrameDecoderBin->m_recvPort;
  }
  return recvPort;
}
/*  HmpFrameDecoderChannel derived class - End
 */


/*  HmpFrameEncoderChannel derived class - Start
 */
HmpFrameEncoderChannel::HmpFrameEncoderChannel(string pipelineName, HmpChannelType pipelineType) :
  HmpVideoChannel(pipelineName, pipelineType) , m_pHmpFrameEncoderBin(NULL)
{
}

HmpFrameEncoderChannel::~HmpFrameEncoderChannel()
{
 XGLOG_INFO( "HmpFrameEncoderChannel::~HmpFrameEncoderChannel destruction start");
  if(m_pHmpFrameEncoderBin) {
    delete m_pHmpFrameEncoderBin;
  }
}

gboolean HmpFrameEncoderChannel::construct_bin(string binName, HmpVideoBinType binType)
{
 XGLOG_INFO( "HmpFrameEncoderChannel::construct_bin %s start", binName.c_str());
  if(!m_pHmpFrameEncoderBin) {
    m_pHmpFrameEncoderBin = new HmpFrameEncoderBin(binName, binType);
    if(!m_pHmpFrameEncoderBin->construct()) {
      XGLOG_ERROR( "construct_bin, HmpFrameEncoderBin construction failed !!");
      return FALSE;
    } else {
      /*gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpFrameEncoderBin->m_bin), NULL); */
    }
  }
  return TRUE;
}

gboolean HmpFrameEncoderChannel::set_ptime(gint ptime)
{
  if(m_pHmpFrameEncoderBin) {
    //return m_pHmpFrameEncoderBin->set_ptime(ptime);
  }
  return FALSE;
}

gint HmpFrameEncoderChannel::get_recv_port()
{
  gint recvPort = 0;
  if(m_pHmpFrameEncoderBin) {
    recvPort = m_pHmpFrameEncoderBin->m_recvPort;
  }
  return recvPort;
}

gboolean HmpFrameEncoderChannel::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
  if(m_pHmpFrameEncoderBin) {
    m_pHmpFrameEncoderBin->add_client_addr(clientIp, clientPort, addExistingClients);
    return TRUE;
  }
  return FALSE;
}
/*  HmpFrameEncoderChannel derived class - End
 */

/* HmpPlayFrameChannel derived class - Start
 */
HmpPlayFrameChannel::HmpPlayFrameChannel(string pipelineName, HmpChannelType pipelineType) :
  HmpVideoChannel(pipelineName, pipelineType) , m_pHmpPlayFrameBin(NULL), m_videoRepeat(FALSE) 
{
}

HmpPlayFrameChannel::~HmpPlayFrameChannel()
{
 XGLOG_INFO( "HmpPlayFrameChannel::~HmpPlayFrameChannel destruction start");
  if(m_pHmpPlayFrameBin) {
    delete m_pHmpPlayFrameBin;
  }
}

gboolean HmpPlayFrameChannel::construct_bin(string binName, HmpVideoBinType binType)
{
 XGLOG_INFO( "HmpPlayAudioChannel:construct_bin %s start", binName.c_str());
  if(!m_pHmpPlayFrameBin) {
    m_pHmpPlayFrameBin = new HmpPlayFrameBin(binName, binType);
    if(!m_pHmpPlayFrameBin->construct()) {
      XGLOG_ERROR( "construct_bin, VoiceSendBin construction failed !!");
      return FALSE;
    } else {
      gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpPlayFrameBin->m_bin), NULL);
    }
  }
  return TRUE;
}

gboolean HmpPlayFrameChannel::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
  if(m_pHmpPlayFrameBin) {
    m_pHmpPlayFrameBin->add_client_addr(clientIp, clientPort, addExistingClients);
    m_pHmpPlayFrameBin->m_sendPort = clientPort;
    return TRUE;
  }
  return FALSE;
}

