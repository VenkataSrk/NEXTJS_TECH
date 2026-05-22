#ifndef _XGATE_HMP_GST_AUDIO_BIN_H
#define _XGATE_HMP_GST_AUDIO_BIN_H



#include "xGateHmpGstUtil.h"
#include "xGateHmpProcessor.h"
#define HOST_AUDIO_PORT_START   10000
#define HOST_AUDIO_PORT_SPAN    20000
int g_audioPort=HOST_AUDIO_PORT_START;
#define HOSTGST_GET_AUDIO_PORT() ((g_audioPort) > (HOST_AUDIO_PORT_SPAN) ? \
    (g_audioPort = HOST_AUDIO_PORT_START) : (g_audioPort = g_audioPort+2));

#define HOST_LOOPBACK_PORT_START   21000
#define HOST_LOOPBACK_PORT_SPAN    30000
int g_loopBackPort=HOST_LOOPBACK_PORT_START;
#define HOSTGST_GET_LOOPBACK_PORT() ((g_loopBackPort) > (HOST_LOOPBACK_PORT_SPAN) ? \
    (g_loopBackPort = HOST_LOOPBACK_PORT_START) : (g_loopBackPort = g_loopBackPort+2));

#define UDP_CAPS_G711U "application/x-rtp, media=(string)audio, clock-rate=(int)8000, encoding-name=(string)PCMU, payload=(int)0"
#define UDP_CAPS_G711A "application/x-rtp, media=(string)audio, clock-rate=(int)8000, encoding-name=(string)PCMA, payload=(int)8"
#define UDP_CAPS_G722 "application/x-rtp, media=(string)audio, clock-rate=(int)8000, encoding-name=(string)G722, payload=(int)9"
#define UDP_CAPS_G729 "application/x-rtp, media=(string)audio, clock-rate=(int)8000, encoding-name=(string)G729, payload=(int)18"
#define UDP_CAPS_OPUS "application/x-rtp,media=(string)audio,clock-rate=(int)48000,encoding-name=(string)X-GST-OPUS-DRAFT-SPITTKA-00"
#define UDP_CAPS_DTLS "application/x-rtp;application/x-dtls;application/x-stun;application/x-srtp"
#define UDP_CAPS_L16 "application/x-rtp, media=(string)audio, payload=(int)[ 96, 127 ], clock-rate=(int)[ 1, 2147483647 ], encoding-name=(string)L16, channels=(int)[ 1, 2147483647 ]; application/x-rtp, media=(string)audio, encoding-name=(string)L16, payload=(int)10, clock-rate=(int)44100; application/x-rtp, media=(string)audio, encoding-name=(string)L16, payload=(int)11, clock-rate=(int)44100"
#define UDP_CAPS_L16 "application/x-rtp, media=(string)audio, clock-rate=(int)8000"

class Client;
//class HmpLocalAudioRecvBin;

/* HmpBin Base class - Start
*/
class HmpBin {
  public:
    //constructor and destuctor
    HmpBin(string binName, HmpBinType binType);
    ~HmpBin();

    //properties
    GstBin *m_bin;
    string m_binName;
    HmpBinType m_binType;

    gint m_recvPort;
    gint m_sendPort;
         
    string m_payload; 

    GSocket *m_recvSocket;
    GSocket *m_sendSocket;

    GstElement *m_udpRecv;
    GstElement *m_udpSend;

    //functionalities
    virtual gboolean construct() = 0;
    virtual gboolean create_udpsrc();
    virtual gboolean create_udpsink();
    virtual gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    virtual gboolean clear_client_addr();
    GstCaps *get_caps();
    gboolean draw_dot_file();
};
/* HmpBin Base class - End
*/

/* HmpVoiceRecvBin derived class - Start
*/
class HmpVoiceRecvBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpVoiceRecvBin(string binName, HmpBinType binType, HmpCodecType codecType, gint recvPort, GSocket *recvSock);
    ~HmpVoiceRecvBin();

    //properties
    HmpCodecType m_codecType;
    GstElement *m_decJitterBuffer, *m_rtpDepay, *m_audioDecoder, *m_audioConvert, *m_audioResample;
    GstPad *m_ghostRtpSrcPad;
    //GstElement *m_dtlssrtpDec, *m_rtpBin, *m_rtpMux;
    //GstElement *m_tee, *m_rtpDtmfDepay, *m_fakeSink, *m_udpDtmfSend; //added for dtmf event to tone conversion 

    //functionalities
    gboolean construct();
    gboolean create_rtpbin();
    gboolean create_elements();
    gboolean add_and_link_elements();
    gboolean register_rtpbin_callback(gpointer data);
    gboolean attach_rtpmux(GstPad *srcPad, gchar *padName);

    //rtpbin element callback functions
    static void on_pad_added(GstElement *rtpBin, GstPad *pad, gpointer data);
    static void on_pad_removed(GstElement *rtpBin, GstPad *pad, gpointer data);
    static GstCaps * on_request_pt_map(GstElement *rtpBin, guint session, guint pt, gpointer data);
};
/* HmpVoiceRecvBin derived class - End
*/

/* HmpDecoderBin derived class - Start
*/
class HmpDecoderBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpDecoderBin(string binName, HmpBinType binType);
    ~HmpDecoderBin();

    //properties
    GstElement *m_decJitterBuffer, *m_rtpDepay, *m_audioDecoder, *m_dtmfDetect, *m_audioRate, *m_audioConvert, *m_fakeSink;
    GstPad *m_ghostRtpSrcPad;

    //functionalities
    gboolean construct();
    gboolean create_decode_elements();
    gboolean add_and_link_elements();
};
/* HmpDecoderBin derived class - End
*/

/* HmpRecordAudioBin derived class - Start
*/
class HmpRecordAudioBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpRecordAudioBin(string binName, HmpBinType binType,HmpCodecType codecType);
    ~HmpRecordAudioBin();

    //properties
    GstElement *m_audioResample, *m_mp3Enc, *m_fileSink1, *m_filter;
    string m_filePath1;
    GstPad *m_ghostRtpSinkPad;
    HmpCodecType m_codecType;   

    //functionalities
    gboolean construct();
    gboolean set_record_file(string &recordFile);
    GstPad * get_tee_dynamic_src_pad();
    GstPad * generate_new_audiomixer_ghostpad();
};

class HmpRecordMP3Bin : public HmpBin {
 public:
     HmpRecordMP3Bin(string binName, HmpBinType binType);
     ~HmpRecordMP3Bin();
     GstElement *m_audioResample, *m_mp3Encoder, *m_fileSink, *m_fileSrc;
     GstElement *m_audioConvert,*m_wavParse;
     gboolean construct();
     std::string m_mp3File;
     std::string m_filePath1;
     gboolean set_record_file(string &recordFile);
};


/* HmpRecordAudioBin derived class - End
*/

/* HmpChannel bass class - Start
*/
class HmpChannel {
  public:
    HmpChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpChannel();

    //public properties
    string m_pipelineName;
    GstElement *m_pipeline;
    HmpChannelType m_pipelineType;
    GstBus *m_bus;
    guint *m_busId;

    //public member functions
    gboolean register_handle_event_callback(gpointer data);
    //static gboolean handle_event(GstBus* bus, GstMessage* msg, gpointer data);
    static gboolean handle_event(GstBus* bus, GstMessage* msg, gpointer data,string CallbackType);
    gboolean draw_dot_file();
    gboolean start();
    gboolean stop();
    gboolean hold();
};
/*  HmpChannel bass class - End
*/

/* HmpVoiceRecvChannel derived class - Start
*/
class HmpVoiceRecvChannel : public HmpChannel {
  public:
    HmpVoiceRecvChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpVoiceRecvChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpBinType binType, HmpCodecType codecType, \
        gint audioPort, GSocket *audioSock);
    gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gboolean clear_client_addr();
    //gboolean add_dtmf_client_addr(string clientIp, gint clientPort);
    //gboolean register_rtpbin_callback(gpointer data);

    gint get_recv_port();
    void set_recv_port(gint port);

    GSocket * get_recv_socket();
    void set_recv_socket(GSocket *socket);

    //gboolean set_stundtlssrtpdec_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId);

    //public member properties
    HmpVoiceRecvBin *m_pHmpVoiceRecvBin;
};
/*  HmpVoiceRecvChannel derived class - End
*/

/* HmpRecordAudioChannel derived class - Start
*/
class HmpRecordAudioChannel : public HmpChannel {
  public:
    HmpRecordAudioChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpRecordAudioChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpBinType binType,HmpCodecType codecType);
    gboolean set_record_file(string &recordFile);
    gboolean add_client(Client *client);
    gboolean schedule_timeout(Client *client);
    static gboolean timeout_handler(gpointer data); 
    gboolean get_record_details(Client *client);
    gboolean send_eos();
    gboolean register_handle_wav_event_callback(gpointer data);
    static gboolean handle_wav_event(GstBus*, GstMessage*, void*);
    //public member properties
    HmpRecordAudioBin *m_pHmpRecordAudioBin;
    guint m_maxTimeoutId;
    guint *m_busId;

  private:
    HmpDecoderBin * create_decoderbin(Client *client); 
    gboolean attach_voice_receive_bin(HmpVoiceRecvBin *hmpVoiceRecvBin); 

    guint m_maxRecordTime;  
};

class HmpRecordMP3Channel : public HmpChannel {
 public:
    HmpRecordMP3Channel(string pipelineName,HmpChannelType pipelineType);
    ~HmpRecordMP3Channel();
    gboolean construct_bin(string binName, HmpBinType binType);
    gboolean set_record_file(string &recordFile);
    static gboolean handle_mp3_event(GstBus*, GstMessage*, void*);
    gboolean register_handle_mp3_event_callback(gpointer data);
    HmpRecordMP3Bin *m_pHmpRecordMP3Bin;
    guint *m_busId;
};
/*  HmpRecordAudioChannel derived class - End
*/

#endif
