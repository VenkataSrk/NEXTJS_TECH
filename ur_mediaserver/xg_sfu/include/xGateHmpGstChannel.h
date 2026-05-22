#ifndef _XGATE_HMP_GST_CHANNEL_H
#define _XGATE_HMP_GST_CHANNEL_H

#include "xGateUtil.h"
#include "xGateHmpGstUtil.h"
#include <gst/app/gstappsrc.h>

#define DEFAULT_RECORD_SCREEN_WIDTH 640
#define DEFAULT_RECORD_SCREEN_HEIGHT 360

class HmpRecordAudioRecvChannel;
class HmpRecordVideoRecvChannel;
class HmpVideoCompositorChannel;

/* HmpGstChannel bass class - Start
*/
class HmpGstChannel {
  public:
    HmpGstChannel(string clientId, string pipelineName, HmpChannelType pipelineType);
    ~HmpGstChannel();

    //public properties
		string m_clientId;
    string m_pipelineName;
    HmpChannelType m_pipelineType;
    GstElement *m_pipeline;
    GstBus *m_bus;
    guint *m_busId;

    //public member functions
    gboolean register_handle_event_callback(gpointer data);
    static gboolean handle_event(GstBus* bus, GstMessage* msg, gpointer data);
    gboolean draw_dot_file();
    gboolean start();
    gboolean pause();
    gboolean stop();
};
/*  HmpGstChannel bass class - End
*/

//TODO: yoga once perfectly build, compiled and tested move this HmpRecordAudioRecvBin class to xGateHmpGstBin.h and cpp files
class HmpGstBin {
  public:
    //constructor and destuctor
    HmpGstBin(string clientId, string binName);
    ~HmpGstBin();

    //properties
		string m_clientId;
		string m_binName;
    GstBin *m_bin;

    gboolean draw_dot_file();
};

/*  HmpRecordAudioRecvBin bass class - End
*/
class HmpRecordAudioRecvBin : public HmpGstBin {
  public:
    //constructor and destuctor
    HmpRecordAudioRecvBin(string clientId, string binName, gint recvPort, GSocket *recvSock, string browser_type);
    ~HmpRecordAudioRecvBin();

    //properties
    GstElement *m_udpSrc, *m_rtpBin, *m_audioMix, *m_opusEnc;
    GstElement *m_capsfilter;
		gint m_recvPort;
		GSocket *m_recvSocket;
    string Browser_Type;

    //functionalities
    gboolean construct();
		gboolean create_udpsrc();
    gboolean create_rtpbin();
		gboolean setup_audiomixer();
    gboolean register_rtpbin_callback(gpointer data);
		void safe_clean();

    //rtpbin element callback functions
    static void on_pad_added(GstElement *rtpBin, GstPad *pad, gpointer data);
    static void on_pad_removed(GstElement *rtpBin, GstPad *pad, gpointer data);
    static GstCaps* on_request_pt_map(GstElement *rtpBin, guint session, guint pt, gpointer data);
    static gboolean attach_depay_decoder(GstPad *pad, gint pt, HmpRecordAudioRecvChannel *channel);

    gboolean register_audio_buffer_probe_callBack (gpointer data, GstElement *element);
    static GstPadProbeReturn handle_AudioPadProbeCallback (GstPad * pad,
                                             GstPadProbeInfo * ProbeInfo, gpointer data);
};
/* HmpRecordAudioRecvBin derived class - End
*/

/* HmpRecordAudioRecvChannel derived class - Start
*/
class HmpRecordAudioRecvChannel : public HmpGstChannel {
  public:
    HmpRecordAudioRecvChannel(string clientId, string pipelineName, HmpChannelType pipelineType,string browser_type);
    ~HmpRecordAudioRecvChannel();

    //public member functions
    gboolean construct_bin(string binName, gint recvPort, GSocket *recvSock, string Browser_Type);
    gboolean register_rtpbin_callback(gpointer data);
    gboolean register_audio_buffer_probe_callBack (gpointer data, GstElement *element);

    gint get_recv_port();
    void set_recv_port(gint port);

    GSocket * get_recv_socket();
    void set_recv_socket(GSocket *socket);
    string Browser_Type;

    //public member properties
    HmpRecordAudioRecvBin *m_pHmpRecordAudioRecvBin;
};
/*  HmpRecordAudioRecvChannel derived class - End
*/

/*  HmpRecordVideoRecvBin bass class - End
*/
class HmpRecordVideoRecvBin : public HmpGstBin {
  public:
    //constructor and destuctor
    HmpRecordVideoRecvBin(string clientId, string binName, gint recvPort, GSocket *recvSock);
    ~HmpRecordVideoRecvBin();

    //properties
    GstElement *m_udpSrc, *m_queue, *m_rtpBin;
		gint m_recvPort;
		GSocket *m_recvSocket;
    guint m_streamCount;
    unsigned int ref_ssrc;

    //functionalities
    gboolean construct();
		gboolean create_udpsrc();
    gboolean create_rtpbin();
    gboolean register_rtpbin_callback(gpointer data);

    //rtpbin element callback functions
    static void on_pad_added(GstElement *rtpBin, GstPad *pad, gpointer data);
    static void on_pad_removed(GstElement *rtpBin, GstPad *pad, gpointer data);
    static GstCaps* on_request_pt_map(GstElement *rtpBin, guint session, guint pt, gpointer data);
		static GstElement* attach_depay_decoder(GstPad *pad, guint &pt, guint &ssrc, 
		       HmpRecordVideoRecvChannel *channel, gpointer data);
		static GstElement* get_new_compositor_pad(HmpRecordVideoRecvChannel *channel);
    static GstPadProbeReturn vp9dec_sink_buffer_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);
};
/* HmpRecordVideoRecvBin derived class - End
*/

/* HmpRecordVideoRecvChannel derived class - Start
*/
class HmpRecordVideoRecvChannel : public HmpGstChannel {
  public:
    HmpRecordVideoRecvChannel(string clientId, string pipelineName, HmpChannelType pipelineType);
    ~HmpRecordVideoRecvChannel();

    //public member functions
    gboolean construct_bin(string binName, gint recvPort, GSocket *recvSock,string Browser_Type);
    gboolean register_rtpbin_callback(gpointer data);

    gint get_recv_port();
    void set_recv_port(gint port);

    GSocket * get_recv_socket();
    void set_recv_socket(GSocket *socket);

    //public member properties
    HmpRecordVideoRecvBin *m_pHmpRecordVideoRecvBin;
		HmpVideoCompositorChannel *m_pHmpCompositorChannel;
};
/*  HmpRecordVideoRecvChannel derived class - End
*/

/*  HmpVideoCompositorBin derived class - Start
*/
class HmpVideoCompositorBin : public HmpGstBin {
  public:
    //constructor and destuctor
    HmpVideoCompositorBin(string clientId, string binName,string Browser_Type);
    ~HmpVideoCompositorBin();

    //properties
    GstElement *m_compositor, *m_videoEnc, *Enc_filter, *m_queue;
    GstElement *m_videoconvert, *m_videoscale;
		guint m_streamCount;
    string Browser_Type;

    //functionalities
    gboolean construct();
    gboolean create_elements();
    gboolean add_and_link_elements();
		GstElement * get_new_compositor_pad(unsigned int ref_ssrc);
		GstPad * get_dynamically_configured_pad();
    gboolean register_video_buffer_probe_callBack (gpointer data, GstElement *element);
    static GstPadProbeReturn handle_VideoPadProbeCallback (GstPad * pad,
                                             GstPadProbeInfo * ProbeInfo, gpointer data);
};
/*  HmpVideoCompositorBin derived class - End
*/

/*  HmpVideoCompositorChannel derived class - Start
*/
class HmpVideoCompositorChannel : public HmpGstChannel {
  public:
    HmpVideoCompositorChannel(string clientId, string pipelineName, HmpChannelType pipelineType);
    ~HmpVideoCompositorChannel();

    //public member functions
    gboolean construct_bin(string binName,string Browser_Type);

    HmpVideoCompositorBin *m_pHmpVideoCompositorBin;
		GstElement * get_new_compositor_pad(unsigned int ref_ssrc);
    gboolean register_video_buffer_probe_callBack (gpointer data, GstElement *element);
};
/*  HmpVideoCompositorChannel derived class - End
*/

/*  HmpAudioVideoMuxRecorderBin derived class - Start
*/
class HmpAudioVideoMuxRecorderBin : public HmpGstBin {
  public:
    //constructor and destuctor
    HmpAudioVideoMuxRecorderBin(string clientId, string binName, string browser_type);
    ~HmpAudioVideoMuxRecorderBin();

    //properties
    GstElement *m_audioSrc, *m_videoSrc, *m_webmMux, *m_fileSink;
    string Browser_Type;
    bool set_audiocaps;
    bool set_videocaps;
    bool is_pipeline_paused;

    //functionalities
    gboolean construct();
    gboolean create_elements();
    gboolean add_and_link_elements();
};
/*  HmpAudioVideoMuxRecorderBin derived class - End
*/

/*  HmpAudioVideoMuxRecorderChannel derived class - Start
*/
class HmpAudioVideoMuxRecorderChannel : public HmpGstChannel {
  public:
    HmpAudioVideoMuxRecorderChannel(string clientId, string pipelineName, HmpChannelType pipelineType);
    ~HmpAudioVideoMuxRecorderChannel();

    //public member functions
    gboolean construct_bin(string binName,string Browser_Type);

    HmpAudioVideoMuxRecorderBin *m_pHmpAudioVideoMuxRecorderBin;
};
/*  HmpAudioVideoMuxRecorderChannel derived class - End
*/
#endif
