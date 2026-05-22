#ifndef _XGATE_HMP_GST_VIDEO_BIN_H
#define _XGATE_HMP_GST_VIDEO_BIN_H

//gstreamer include
extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <gst/gst.h>
#include <fcntl.h>
#include <glib.h>
#include <gio/gio.h>
#include <gst/net/gstnet.h>
}

//local include
#include "xGateUtil.h"
#include "xGateHmpGstUtil.h"
#include "xGateMgMsg.h"

#define GST_DEBUG TRUE

#define HOST_VIDEO_PORT_START   31000
#define HOST_VIDEO_PORT_SPAN    40000
int g_videoPort=HOST_VIDEO_PORT_START;
#define HOSTGST_GET_VIDEO_PORT() ((g_videoPort) > (HOST_VIDEO_PORT_SPAN) ? \
    (g_videoPort = HOST_VIDEO_PORT_START) : (g_videoPort = g_videoPort+2));

#define HOST_VIDEO_LOOPBACK_PORT_START   41000
#define HOST_VIDEO_LOOPBACK_PORT_SPAN    50000
int g_videoLoopBackPort=HOST_VIDEO_LOOPBACK_PORT_START;
#define HOSTGST_GET_VIDEO_LOOPBACK_PORT() ((g_videoLoopBackPort) > (HOST_VIDEO_LOOPBACK_PORT_SPAN) ? \
    (g_videoLoopBackPort = HOST_VIDEO_LOOPBACK_PORT_START) : (g_videoLoopBackPort = g_videoLoopBackPort+2));

#define UDP_CAPS_VID_VP8 "application/x-rtp, clock-rate=(int)90000,payload=(int)125, media=(string)video, encoding-name=(string)VP8"
#define UDP_CAPS_VID_VP9 "application/x-rtp, clock-rate=(int)90000,payload=(int)127, media=(string)video, encoding-name=(string)VP9" 
#define UDP_CAPS_VID_H264 "application/x-rtp, clock-rate=(int)90000,payload=(int)121, media=(string)video, encoding-name=(string)H264"
#define UDP_CAPS_VID_H265 "application/x-rtp, clock-rate=(int)90000,payload=(int)123, media=(string)video, encoding-name=(string)H265"


/* HmpVideoBin Base class - Start
 */
class HmpVideoBin {
  public:
    /*constructor and destuctor */
    HmpVideoBin(string binName, HmpVideoBinType binType);
    ~HmpVideoBin();
 
    /*properties */
    GstBin *m_bin;
    string m_binName;
    HmpVideoBinType m_vbinType;

    gint m_recvPort;
    gint m_sendPort;

    GSocket *m_recvSocket;
    GSocket *m_sendSocket;

    GstElement *m_udpRecv;
    GstElement *m_udpSend;

    /*functionalities */
    virtual gboolean construct() = 0;
    virtual gboolean create_udpsrc();
    virtual gboolean create_udpsink();
    virtual gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    GstCaps *get_video_caps();
    gboolean draw_dot_file();
};
/* HmpVideoBin Base class - End
 */

/* HmpFrameRecvBin derived class - Start
*/
class HmpFrameRecvBin : public HmpVideoBin {
  public:
    //constructor and destuctor 
    HmpFrameRecvBin(string binName, HmpVideoBinType binType, gint recvPort, GSocket *recvSock);
    ~HmpFrameRecvBin();

    //properties 
    GstElement *m_dtlssrtpDec, *m_rtpBin;

    //functionalities  
    gboolean construct();
    gboolean create_rtpbin();
    gboolean create_elements();
    gboolean add_and_link_elements();
    gboolean register_rtpbin_callback(gpointer data);
    gboolean set_stundtlssrtpdec_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId);

    //rtpbin element callback functions  
    //static void on_pad_added(GstElement *rtpBin, GstPad *pad, gpointer data);
    //static void on_pad_removed(GstElement *rtpBin, GstPad *pad, gpointer data);
    //static GstCaps * on_request_pt_map(GstElement *rtpBin, guint session, guint pt, gpointer data);
};
/* HmpFrameRecvBin derived class - End
 */

/* HmpFrameSendBin derived class - Start
 */
class HmpFrameSendBin : public HmpVideoBin {
  public:
    //constructor and destuctor 
    HmpFrameSendBin(string binName, HmpVideoBinType binType, GSocket *sendSocket);
    ~HmpFrameSendBin();

    //properties 
    GstElement *m_dtlssrtpEnc;
    string m_sendIp; //i.e client ip  

    //functionalities  
    gboolean construct();
    gboolean create_elements();
    gboolean add_and_link_elements();
    gboolean set_stundtlssrtpenc_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId);
};
/* HmpVoiceSendBin derived class - End
 */

/* HmpFrameDecoderBin derived class - Start
 */
class HmpFrameDecoderBin : public HmpVideoBin {
  public:
    /*constructor and destuctor */
    HmpFrameDecoderBin(string binName, HmpVideoBinType binType);
    ~HmpFrameDecoderBin();

    /*properties */
    GstElement *m_videoRtpDepay, *m_videoDecoder, *m_videoConvert;
    GstPad *m_ghostRtpSrcPad;

    /*functionalities */
    gboolean construct();
    gboolean create_decode_elements();
    gboolean add_and_link_elements();
};
/* HmpDecoderBin derived class - End
 */

/* HmpFrameDecoderBin derived class - Start
 */
class HmpFrameEncoderBin : public HmpVideoBin {
  public:
    /*constructor and destuctor */
    HmpFrameEncoderBin(string binName, HmpVideoBinType binType);
    ~HmpFrameEncoderBin();

    /*properties */
    GstElement *m_videoScale, *m_videoRate, *m_videoEncoder, *m_videoRtpPay;
    GstPad *m_ghostRtpSinkPad;

    /*functionalities */
    gboolean construct();
    gboolean create_encode_elements();
    gboolean add_and_link_elements();
    gboolean set_ptime(gint ptime);
};
/* HmpFrameEncoderBin derived class - End
 */

/* HmpPlayAudioBin derived class - Start
 */
class HmpPlayFrameBin : public HmpVideoBin {
  public:
    //constructor and destuctor
    HmpPlayFrameBin(string binName, HmpVideoBinType binType);
    ~HmpPlayFrameBin();

    //properties
    GstElement *m_fileSrc, *m_rawVideoParse, *m_videoConvert, *m_videoScale, *m_videoEncoder, *m_videoRtpPay;
    HmpCodecType m_codecType;

    //functionalities
    void set_codec_type(HmpCodecType codecType) {
      m_codecType = codecType;
    }

    //functionalities
    gboolean construct();
    gboolean create_encode_elements();
    gboolean add_and_link_elements();
    gboolean set_play_file(string playFile);
};
/* HmpPlayFrameBin derived class - End
 */

/* HmpVideoChannel bass class - Start
 * */
class HmpVideoChannel {
  public:
    HmpVideoChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpVideoChannel();

    //public properties
    string m_pipelineName;
    GstElement *m_pipeline;
    HmpChannelType m_pipelineType;
    GstBus *m_bus;
    guint *m_busId;

    //public member functions
    gboolean register_handle_event_callback(gpointer data);
    static gboolean handle_event(GstBus* bus, GstMessage* msg, gpointer data);
    gboolean draw_dot_file();
    gboolean start();
    gboolean stop();
};
/*  HmpChannel bass class - End
 */

/* HmpFrameRecvChannel derived class - Start
 */
class HmpFrameRecvChannel : public HmpVideoChannel {
  public:
   HmpFrameRecvChannel(string pipelineName, HmpChannelType pipelineType);
   ~HmpFrameRecvChannel();
   
   //public member functions
   gboolean construct_bin(string binName, HmpVideoBinType binType, gint recvPort, GSocket *recvSock);
   gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
   gboolean register_rtpbin_callback(gpointer data);

   gint get_recv_port();
   void set_recv_port(gint port);

   GSocket * get_recv_socket();
   void set_recv_socket(GSocket *socket);

   gboolean set_stundtlssrtpdec_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId);

   //public member properties
   HmpFrameRecvBin *m_pHmpFrameRecvBin;
};
/*  HmpFrameRecvChannel derived class - End
 */

/* HmpFrameSendChannel derived class - Start
 */
class HmpFrameSendChannel : public HmpVideoChannel {
  public:
    HmpFrameSendChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpFrameSendChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpVideoBinType binType, GSocket *sendSocket);
    gint get_recv_port();
    gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gboolean set_stundtlssrtpenc_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId);

    //public member properties
    HmpFrameSendBin *m_pHmpFrameSendBin;
};
/*  HmpFrameSendChannel derived class - End
 */
        
/*  HmpFrameDecoderChannel derived class - Start
 */
class HmpFrameDecoderChannel : public HmpVideoChannel {
  public:
    HmpFrameDecoderChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpFrameDecoderChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpVideoBinType binType);
    gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gint get_recv_port();

    //public member properties
    HmpFrameDecoderBin *m_pHmpFrameDecoderBin;
};
/*  HmpFrameDecoderChannel derived class - End
 */


/*  HmpFrameEncoderChannel derived class - Start
 */
class HmpFrameEncoderChannel : public HmpVideoChannel {
  public:
    HmpFrameEncoderChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpFrameEncoderChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpVideoBinType binType);
    gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gint get_recv_port();
    gboolean set_ptime(gint ptime);

    //public member properties
    HmpFrameEncoderBin *m_pHmpFrameEncoderBin;
};
/*  HmpEncoderChannel derived class - End
 */

/* HmpPlayFrameChannel derived class - Start
 */
class HmpPlayFrameChannel : public HmpVideoChannel {
  public:
    HmpPlayFrameChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpPlayFrameChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpVideoBinType binType);
    gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gboolean set_play_attributes(string playFile, gboolean autoRepeat);
    gboolean post_playframe_eos(Client *client);

    //public member properties
    HmpPlayFrameBin *m_pHmpPlayFrameBin;
    gboolean m_videoRepeat;
};
/*  HmpPlayFrameChannel derived class - End
 */

#endif
