#ifndef _XGATE_HMP_GST_AUDIO_BIN_H
#define _XGATE_HMP_GST_AUDIO_BIN_H

#include "xGateHmpGstUtil.h"
#include "xGateHmpGstBin.h"
#include "xGateHmpGstChannel.h"

#define HOST_AUDIO_PORT_START   10000
#define HOST_AUDIO_PORT_SPAN    20000
int g_audioPort=HOST_AUDIO_PORT_START;
#define HOSTGST_GET_AUDIO_PORT() ((g_audioPort) > (HOST_AUDIO_PORT_SPAN) ? \
    (g_audioPort = (HOST_AUDIO_PORT_START+(xGateSFUtil::getSFUId()*4))) : (g_audioPort = (g_audioPort+2+(xGateSFUtil::getSFUId()*4))));

#define HOST_LOOPBACK_PORT_START   21000
#define HOST_LOOPBACK_PORT_SPAN    30000
int g_loopBackPort=HOST_LOOPBACK_PORT_START;
#define HOSTGST_GET_LOOPBACK_PORT() ((g_loopBackPort) > (HOST_LOOPBACK_PORT_SPAN) ? \
    (g_loopBackPort = (HOST_LOOPBACK_PORT_START+(xGateSFUtil::getSFUId()*7))) : (g_loopBackPort = (g_loopBackPort+2+(xGateSFUtil::getSFUId()*7))));
#define UDP_CAPS_G711U "application/x-rtp, media=(string)audio, clock-rate=(int)8000, encoding-name=(string)PCMU, payload=(int)0"
#define UDP_CAPS_G711A "application/x-rtp, media=(string)audio, clock-rate=(int)8000, encoding-name=(string)PCMA, payload=(int)8"
#define UDP_CAPS_G722 "application/x-rtp, media=(string)audio, clock-rate=(int)8000, encoding-name=(string)G722, payload=(int)9"
#define UDP_CAPS_G729 "application/x-rtp, media=(string)audio, clock-rate=(int)8000, encoding-name=(string)G729, payload=(int)18"
#define UDP_CAPS_OPUS "application/x-rtp,media=(string)audio,clock-rate=(int)48000, encoding-name=(string)OPUS, encoding-params=(string)2, payload=(int)111"
#define UDP_CAPS_DTLS "application/x-rtp;application/x-dtls;application/x-stun;application/x-srtp"
#define UDP_CAPS_L16 "application/x-rtp, media=(string)audio, payload=(int)[ 96, 127 ], clock-rate=(int)[ 1, 2147483647 ], encoding-name=(string)L16, channels=(int)[ 1, 2147483647 ]; application/x-rtp, media=(string)audio, encoding-name=(string)L16, payload=(int)10, clock-rate=(int)44100; application/x-rtp, media=(string)audio, encoding-name=(string)L16, payload=(int)11, clock-rate=(int)44100"
#define UDP_CAPS_L16 "application/x-rtp, media=(string)audio, clock-rate=(int)8000"
#define CAPS_STR_FOR_AUDIODEC "audio/x-raw, format=(string)S16LE, rate=(int)48000, channels=(int)2, layout=(string)interleaved"
class Client;
class HmpLocalAudioRecvBin;
//unsigned char *AUDIO_CAPS[] = {UDP_CAPS_OPUS,UDP_CAPS_G711U,UDP_CAPS_G711A,UDP_CAPS_G722,UDP_CAPS_G729};
unsigned char *AUDIO_CAPS[] = {UDP_CAPS_OPUS};

/* HmpBin Base class - Start
*/
class HmpBin {
  public:
    //constructor and destuctor
    HmpBin(string binName, HmpBinType binType, unsigned int m_dynamicCodec);
    ~HmpBin();

    //properties
    GstBin *m_bin;
    string m_binName;
    HmpBinType m_binType;

    gint m_recvPort;
    gint m_sendPort;

    GSocket *m_audioSocket;
    GSocket *m_videoSocket;
    GSocket *m_recvSocket;
    GSocket *m_sendSocket;

    GstElement *m_udpRecv;
    GstElement *m_udpSend;
    unsigned int m_dynamicCodec;

    //functionalities
    virtual gboolean construct() = 0;
    virtual gboolean create_udpsrc();
    // virtual gboolean create_Videoudpsrc();
    // virtual gboolean create_Audioudpsrc();
    virtual gboolean create_udpsink();
    virtual gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    virtual gboolean clear_client_addr();
    GstCaps *get_caps();
    gboolean draw_dot_file();
};
/* HmpBin Base class - End
*/

/* HmpRecordAudioBin derived class - Start
*/
class HmpRecordAudioBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpRecordAudioBin(string binName, HmpBinType binType, GSocket *sendSocket);
    ~HmpRecordAudioBin();

    //properties
    string m_sendIp; //i.e SRS ip
    gint m_srcPort;

    //functionalities
    gboolean construct();
    gboolean add_and_link_elements();
};
/* HmpRecordAudioBin derived class - End
*/

//HmpMixerBin derived class - Start
class HmpMixerBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpMixerBin(string binName, HmpBinType binType);
    ~HmpMixerBin();

    //properties
    GstElement *m_capsfilter, *m_audioadder, *m_audiodepay;
    GstElement *m_opusenc, *m_appsink;

    unsigned int m_audioPayload, m_videoPayload;
    //functionalities
    gboolean construct();
    gboolean create_mix_elements();
    gboolean add_and_link_mix_elements();
    GstPad *generate_new_audiomixer_Ghostpad();
    gboolean register_appsink_callback(gpointer data);
    static gboolean on_new_data_rcvd_appsink(GstElement *appsink, gpointer data);
};

/* HmpChannel bass class - Start
*/
class HmpChannel {
  public:
    HmpChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpChannel();

    //public properties
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
    gboolean stop();
};
/*  HmpChannel bass class - End
*/

/* HmpRecordAudioChannel derived class - Start
*/
class HmpRecordAudioChannel : public HmpChannel {
  public:
    HmpRecordAudioChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpRecordAudioChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpBinType binType, GSocket *sendSocket);
    gint get_recv_port();
    gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gboolean clear_client_addr();

    //public member properties
    HmpRecordAudioBin *m_pHmpRecordAudioBin;

  private:
     gboolean add_and_link_elements();
};
/*  HmpRecordAudioChannel derived class - End
*/

class HmpAudioDecoderBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpAudioDecoderBin(string binName, HmpBinType binType, HmpCodecType codecType);
    ~HmpAudioDecoderBin();

    //functionalities
    HmpCodecType m_audioCodecType;
    gboolean construct();
    gboolean create_decoderbin();
    static xGateRetVal on_new_data_rcvd_appsink (GstElement *appsink, gpointer data);

  //decoder elements for audio
    GstElement *m_opusdepay, *m_opusdec, *m_opusdec_queue;
    GstElement *m_appsinkAudio;
    GstElement *m_capsfilter;
    unsigned long m_ulNumAudioPktRcvd;
};

#if 0
//HmpAudioVideoMixerBin derived class - Start
class HmpAudioVideoMixerBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpAudioVideoMixerBin(string binName, HmpBinType binType);
    ~HmpAudioVideoMixerBin();

    //properties
    GstElement *m_webmmux, *m_fileSink;
    GstElement *m_appsrc,*m_video_appsrc,*m_id;

    //functionalities
    gboolean construct();
    gboolean create_elements();
    gboolean add_and_link_elements();
};

/* HmpAudioVideoMixerChannel derived class - Start */
class HmpAudioVideoMixerChannel : public HmpChannel {
  public:
    HmpAudioVideoMixerChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpAudioVideoMixerChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpBinType binType);

    HmpAudioVideoMixerBin *m_pHmpAudioVideoMixerBin;
};
/* HmpAudioVideoMixerChannel derived class - Stop */
#endif

#endif
