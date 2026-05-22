#ifndef _XGATE_HMP_GST_AUDIO_BIN_H
#define _XGATE_HMP_GST_AUDIO_BIN_H

#include "xGateHmpGstUtil.h"
#include "xGateHmpGstBin.h"

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
//#define UDP_CAPS_OPUS "application/x-rtp,media=(string)audio,clock-rate=(int)48000, encoding-name=(string)OPUS, encoding-params=(string)2, payload=(int)111"
#define UDP_CAPS_OPUS "application/x-rtp,media=(string)audio,clock-rate=(int)48000, encoding-name=(string)OPUS, encoding-params=(string)2, payload=(int)"
#define UDP_CAPS_DTLS "application/x-rtp;application/x-dtls;application/x-stun;application/x-srtp"
#define UDP_CAPS_L16 "application/x-rtp, media=(string)audio, payload=(int)[ 96, 127 ], clock-rate=(int)[ 1, 2147483647 ], encoding-name=(string)L16, channels=(int)[ 1, 2147483647 ]; application/x-rtp, media=(string)audio, encoding-name=(string)L16, payload=(int)10, clock-rate=(int)44100; application/x-rtp, media=(string)audio, encoding-name=(string)L16, payload=(int)11, clock-rate=(int)44100"
#define UDP_CAPS_L16 "application/x-rtp, media=(string)audio, clock-rate=(int)8000"

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

    GSocket *m_recvSocket;
    GSocket *m_sendSocket;

    GstElement *m_udpRecv;
    GstElement *m_udpSend;
    unsigned int m_dynamicCodec;

    //functionalities
    virtual gboolean construct() = 0;
    virtual gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gboolean draw_dot_file();
#if 0
    virtual gboolean create_udpsrc();
    virtual gboolean create_udpsink();
    virtual gboolean clear_client_addr();
    GstCaps *get_caps();
#endif
};
/* HmpBin Base class - End
*/

/* HmpVoiceRecvBin derived class - Start
*/
class HmpVoiceRecvBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpVoiceRecvBin(string binName, HmpBinType binType, HmpCodecType codecType, gint recvPort, GSocket *recvSock, unsigned int dynamicCodec);
    ~HmpVoiceRecvBin();

    //properties
    HmpCodecType m_codecType;
    unsigned int m_dynamicCodec;
    GstElement *m_dtlssrtpDec, *m_rtpBin, *m_rtpMux;
    GstElement *m_tee, *m_rtpDtmfDepay, *m_fakeSink, *m_udpDtmfSend; //added for dtmf event to tone conversion 

    //functionalities
    gboolean construct();
    gboolean create_rtpbin();
    gboolean create_elements();
    gboolean add_and_link_elements();
    gboolean register_rtpbin_callback(gpointer data);
    gboolean set_stundtlssrtpdec_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId);
    gboolean attach_rtpmux(GstPad *srcPad, gchar *padName, gboolean isDtmfPad = FALSE);
    GstPad * get_tee_dynamic_src_pad();
    gboolean add_dtmf_client_addr(string clientIp, gint clientPort);

    //rtpbin element callback functions
    static void on_pad_added(GstElement *rtpBin, GstPad *pad, gpointer data);
    static void on_pad_removed(GstElement *rtpBin, GstPad *pad, gpointer data);
    static GstCaps * on_request_pt_map(GstElement *rtpBin, guint session, guint pt, gpointer data);

};
/* HmpVoiceRecvBin derived class - End
*/

/* HmpVoiceSendBin derived class - Start
*/
class HmpVoiceSendBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpVoiceSendBin(string binName, HmpBinType binType, GSocket *sendSocket, unsigned int dynamicCodec);
    ~HmpVoiceSendBin();

    //properties
    GstElement *m_dtlssrtpEnc, *m_rtpBin;
    string m_sendIp; //i.e client ip

    //functionalities
    gboolean construct();
    gboolean create_rtpbin();
    gboolean create_elements();
    gboolean add_and_link_elements();
    gboolean set_stundtlssrtpenc_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId);

    gboolean register_rtpbin_callback(gpointer data);
    static void on_pad_added(GstElement *rtpBin, GstPad *pad, gpointer data);
    static GstCaps * on_request_pt_map(GstElement *rtpBin, guint session, guint pt, gpointer data);
};
/* HmpVoiceSendBin derived class - End
*/

/* HmpDecoderBin derived class - Start
*/
class HmpDecoderBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpDecoderBin(string binName, HmpBinType binType, unsigned int dynamicCodec);
    ~HmpDecoderBin();

    //properties
    GstElement *m_decJitterBuffer, *m_rtpDepay, *m_audioDecoder, *m_dtmfDetect, *m_audioRate, *m_audioConvert, *m_fakeSink;
    GstPad *m_ghostRtpSrcPad;
    GstPad *audiomix_ghost_sinkpad;

    //functionalities
    gboolean construct();
    gboolean create_decode_elements();
    gboolean add_and_link_elements();
};
/* HmpDecoderBin derived class - End
*/

/* HmpDecoderBin derived class - Start
*/
class HmpEncoderBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpEncoderBin(string binName, HmpBinType binType, unsigned int m_dynamicCodec);
    ~HmpEncoderBin();

    //properties
    GstElement *m_audioConvert, *m_encoder, *m_rtpPay;
    GstPad *m_ghostRtpSinkPad;

    //functionalities
    gboolean construct();
    gboolean create_encode_elements();
    gboolean add_and_link_elements();
    gboolean set_ptime(gint ptime);
    gboolean start_encode();
    gboolean stop_encode();
};
/* HmpDecoderBin derived class - End
*/

/* HmpPlayAudioBin derived class - Start
*/
class HmpPlayAudioBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpPlayAudioBin(string binName, HmpBinType binType, unsigned int dynamicCodec);
    ~HmpPlayAudioBin();

    //properties
    GstElement *m_fileSrc, *m_wavParse, *m_audioConvert, *m_audioResample, *m_payEnc, *m_rtpPay;
    HmpCodecType m_codecType;

    //functionalities
    void set_codec_type(HmpCodecType codecType) {
      m_codecType = codecType;
    }

    //functionalities
    gboolean construct();
    gboolean set_ptime(gint ptime);
    gboolean set_play_file(string playFile);
};
/* HmpPlayAudioBin derived class - End
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

/* HmpLocalAudioRecvBin derived class - Start
*/
class HmpLocalAudioRecvBin : public HmpBin {
  public:
    HmpLocalAudioRecvBin(string binName, HmpBinType binType);
    ~HmpLocalAudioRecvBin();
    gboolean construct();
    gint get_recv_port();

    GstElement *m_rtpL16depay, *m_audioConvert;
    GstPad *m_ghostSrcPad;

  private:
    gboolean create_elements();
    gboolean add_and_link_elements();
};
/* HmpLocalAudioRecvBin derived class - Start
*/
/* HmpLocalAudioSendBin bass class - Start
 *  */
class HmpLocalAudioSendBin : public HmpBin {
  public:
    HmpLocalAudioSendBin(string binName, HmpBinType binType);
    ~HmpLocalAudioSendBin();
    gboolean construct();

    GstElement *m_audioRate, *m_audioConvert, *m_rtpL16pay;
    GstPad *m_ghostSinkPad;

  private:
    gboolean create_elements();
    gboolean add_and_link_elements();

};

/* HmpMRCPVoiceCaptureBin derived class - Start
*/
class HmpMRCPVoiceCaptureBin : public HmpBin {
  public:
    //constructor and destuctor
    HmpMRCPVoiceCaptureBin(string binName, HmpBinType binType,xGateHmpClientAsrType asrType,unsigned int audioPayload);
    ~HmpMRCPVoiceCaptureBin();

    //properties
    GstElement *m_decJitterBuffer, *m_rtpDepay, *m_audioDecoder;
    GstElement *m_audioConvert, *m_mrcp_filter, *m_audioResample,*m_filter;
    xGateHmpClientAsrType m_asrType;
    unsigned int m_audioPayload;

    //functionalities
    gboolean construct();
    gboolean create_decode_elements();
    gboolean add_and_link_elements();
    gboolean register_buffer_probe_callBack (gpointer data, GstElement *element);
    static GstPadProbeReturn handle_PadProbeCallback (GstPad * pad,
                                             GstPadProbeInfo * ProbeInfo, gpointer data);
    gboolean send_stream_start_event (GstPad *sink_pad, gchar *u8StreamId);
    gboolean send_segment_event(GstPad *sinkPad);
};




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
    static gboolean handle_event(GstBus* bus, GstMessage* msg, gpointer data);
    gboolean draw_dot_file();
    gboolean start();
    gboolean stop();
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
        gint audioPort, GSocket *audioSock,  unsigned int dynamicCodec);
    gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gboolean clear_client_addr();
    gboolean add_dtmf_client_addr(string clientIp, gint clientPort);
    gboolean register_rtpbin_callback(gpointer data);

    gint get_recv_port();
    void set_recv_port(gint port);

    GSocket * get_recv_socket();
    void set_recv_socket(GSocket *socket);

    gboolean set_stundtlssrtpdec_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId);

    //public member properties
    HmpVoiceRecvBin *m_pHmpVoiceRecvBin;
};
/*  HmpVoiceRecvChannel derived class - End
*/

/* HmpVoiceSendChannel derived class - Start
*/
class HmpVoiceSendChannel : public HmpChannel {
  public:
    HmpVoiceSendChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpVoiceSendChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpBinType binType, GSocket *sendSocket, unsigned int dynamicCodec);
    gint get_recv_port();
    gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gboolean clear_client_addr();
    gboolean set_stundtlssrtpenc_attributes(MediaSecureDetail &secureDetail, string &stundtlsConnId);

    gboolean register_rtpbin_callback(gpointer data);

    //public member properties
    HmpVoiceSendBin *m_pHmpVoiceSendBin;

};
/*  HmpVoiceSendChannel derived class - End
*/

/*  HmpDecoderChannel derived class - Start
*/
class HmpDecoderChannel : public HmpChannel {
  public:
    HmpDecoderChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpDecoderChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpBinType binType, unsigned int dynamicCodec);
    gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gboolean clear_client_addr();
    gint get_recv_port();
    gboolean add_decoder_sendbin();

    //public member properties
    HmpDecoderBin *m_pHmpDecoderBin;
    HmpLocalAudioSendBin *m_audioSendBin;
};
/*  HmpDecoderChannel derived class - End
*/

/*  HmpEncoderChannel derived class - Start
*/
class HmpEncoderChannel : public HmpChannel {
  public:
    HmpEncoderChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpEncoderChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpBinType binType, unsigned int dynamicCodec);
    gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gboolean clear_client_addr();
    gint get_recv_port();
    gboolean set_ptime(gint ptime);

    //public member properties
    HmpEncoderBin *m_pHmpEncoderBin;
    gboolean start_encode();
    gboolean stop_encode();
};
/*  HmpEncoderChannel derived class - End
*/


/* HmpPlayAudioChannel derived class - Start
*/
class HmpPlayAudioChannel : public HmpChannel {
  public:
    HmpPlayAudioChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpPlayAudioChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpBinType binType, HmpCodecType codecType, unsigned int dynamicCodec);
    gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gboolean clear_client_addr();
    gboolean set_ptime(gint ptime);
    gboolean set_play_attributes(string playFile, gboolean autoRepeat);
    gboolean post_playaudio_eos(Client *client);

    //public member properties
    HmpPlayAudioBin *m_pHmpPlayAudioBin;
    gboolean m_autoRepeat;
};
/*  HmpPlayAudioChannel derived class - End
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

/* HmpMRCPVoiceCaptureChannel derived class - Start
*/
class HmpMRCPVoiceCaptureChannel : public HmpChannel {
  public:
    HmpMRCPVoiceCaptureChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpMRCPVoiceCaptureChannel();

    //public member functions
    gboolean construct_bin(string binName, HmpBinType binType,xGateHmpClientAsrType asrType,unsigned int audioPayload);
    gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
    gboolean register_appSink_callBack(gpointer);
    gboolean register_buffer_probe_callBack (gpointer data, GstElement *element);
    GstPadProbeReturn handle_PadProbeCallback (GstPad * pad,
                                             GstPadProbeInfo * ProbeInfo, gpointer data);
    //public member properties
    HmpMRCPVoiceCaptureBin *m_pHmpMRCPVoiceCaptureBin;
};



#endif
