#ifndef _XGATE_HMP_GST_AUDIO_BIN_H
#define _XGATE_HMP_GST_AUDIO_BIN_H


#include "xGateHmpGstUtil.h"
#include <gst/rtp/gstrtcpbuffer.h>
#define HOST_AUDIO_PORT_START   10000
#define HOST_AUDIO_PORT_SPAN    28000
int g_audioPort=HOST_AUDIO_PORT_START;
#define HOSTGST_GET_AUDIO_PORT() ((g_audioPort) > (HOST_AUDIO_PORT_SPAN) ? \
      (g_audioPort = HOST_AUDIO_PORT_START) : (g_audioPort = g_audioPort+2));

#define GST_DEBUG_DRAW_GRAPH 1
#define HOST_LOOPBACK_PORT_START   30000
#define HOST_LOOPBACK_PORT_SPAN    50000
int g_loopBackPort=HOST_LOOPBACK_PORT_START;
#define HOSTGST_GET_LOOPBACK_PORT() ((g_loopBackPort) > (HOST_LOOPBACK_PORT_SPAN) ? \
      (g_loopBackPort = HOST_LOOPBACK_PORT_START) : (g_loopBackPort = g_loopBackPort+1));

#define UDP_CAPS_G711U "application/x-rtp,encoding-name=(string)PCMU,clock-rate=(int)8000,payload=(int)0,media=(string)audio"
#define UDP_CAPS_G711U_TTS "application/x-rtp,encoding-name=(string)PCMU,clock-rate=(int)22050,payload=(int)0,media=(string)audio"
#define UDP_CAPS_G711A "application/x-rtp,encoding-name=(string)PCMA,clock-rate=(int)8000,payload=(int)8,media=(string)audio"
#define UDP_CAPS_G711A_TTS "application/x-rtp,encoding-name=(string)PCMA,clock-rate=(int)22050,payload=(int)8,media=(string)audio"
#define UDP_CAPS_G722 "application/x-rtp,encoding-name=(string)G722,clock-rate=(int)8000,payload=(int)9,media=(string)audio"
#define UDP_CAPS_G729 "application/x-rtp, media=(string)audio, clock-rate=(int)8000, encoding-name=(string)G729, payload=(int)18"
#define UDP_CAPS_OPUS "application/x-rtp,encoding-name=(string)X-GST-OPUS-DRAFT-SPITTKA-00,clock-rate=(int)48000,media=(string)audio, payload=(int)107"
#define UDP_CAPS_DTLS "application/x-rtp;application/x-dtls;application/x-stun;application/x-srtp"
#define UDP_CAPS_L16 "application/x-rtp, media=(string)audio, clock-rate=(int)8000"

class Client;

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
      string m_payload;

      gint m_recvPort;
      gint m_sendPort;

      GSocket *m_recvSocket;
      GSocket *m_sendSocket;

      GstElement *m_udpRecv;
      GstElement *m_udpSend;

      //functionalities
      virtual gboolean construct() = 0;
      virtual gboolean create_udpsrc(gboolean isTTS=FALSE);
      virtual gboolean create_udpsink(gboolean flag = FALSE);
      virtual gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
      virtual gboolean clear_client_addr();
      virtual string get_existing_addr();
      virtual gboolean set_existing_addr(string existingAddr);
      GstCaps *get_caps(gboolean isTTS=FALSE);
      gboolean draw_dot_file();
};
/* HmpBin Base class - End
*/

/* HmpVoiceRecvDecoderBin derived class - Start
*/
class HmpVoiceRecvDecoderBin : public HmpBin {
   public:
      //constructor and destuctor
      HmpVoiceRecvDecoderBin(string binName, HmpBinType binType, xGateTTSType ttsType,gint recvPort, GSocket *recvSock,gboolean isTTS=FALSE);
      ~HmpVoiceRecvDecoderBin();

      //properties
      xGateTTSType m_ttsType;
      GstElement *m_decJitterBuffer, *m_rtpDepay, *m_audioDecoder;
      GstElement *m_audioRate, *m_audioResample, *m_audioConvert;
      GstElement *m_id1,*m_id2,*m_id3,*m_id4;
      GstElement* m_dtlssrtpDec;
      GstPad* m_sink_app;
      GstPad *m_ghostRtpSrcPad;
      GstPad *audioconvert_ghost_sinkpad;
      gboolean m_isSTTpipeline;
      gboolean m_ttsFlag;
      gboolean m_in_pipeline;
      gboolean m_rem_client;

      //functionalities
      gboolean construct();
      gboolean create_decode_elements();
      gboolean add_and_link_elements();
      gboolean remove_and_unlink_elements();
      gboolean register_identity_callback(gpointer data);
      gboolean set_handoff();
      static void identity_callback (GstElement * identity,GstBuffer * buffer,gpointer data);
};
/* HmpVoiceRecvDecoderBin derived class - End
*/

/* HmpVoiceSendEncoderBin derived class - Start
*/
class HmpVoiceSendEncoderBin : public HmpBin {
   public:
      //constructor and destuctor
      HmpVoiceSendEncoderBin(string binName, HmpBinType binType,xGateTTSType ttsType,HmpCodecType codecType, GSocket *sendSocket);
      ~HmpVoiceSendEncoderBin();

      //properties
      xGateTTSType m_ttsType;
      GstElement *m_appSrc,*m_capsFilter;
      GstElement *m_audioRate, *m_audioConvert, *m_encoder, *m_rtpPay;
      GstElement *m_id5,*m_id6,*m_id7;
      HmpCodecType m_codecType; 
      GstPad *m_ghostRtpSinkPad;
      string m_sendIp; //i.e client ip
      gboolean m_is_ipv6;
      short m_ptime;
      gboolean m_in_pipeline;
      gboolean m_rem_client;

      //functionalities
      gboolean construct();
      gboolean create_encode_elements();
      gboolean add_and_link_elements();
      gboolean set_ptime(gint ptime, gint maxPtime);
      gboolean register_identity_callback(gpointer data);
      gboolean set_handoff();
      static void identity_callback (GstElement * identity,GstBuffer * buffer,gpointer data);
};
/* HmpVoiceSendEncoderBin derived class - End
*/

/* HmpVoiceRePacketizeBin derived class - Start
*/
class HmpVoiceRePacketizeBin : public HmpBin
{
   public:
      //constructor and destuctor
      ~HmpVoiceRePacketizeBin();
      HmpVoiceRePacketizeBin(string binName, HmpBinType binType,HmpCodecType codecType, gint recvPort, GSocket *recvSock);

      //properties
      GstElement *m_rtpDepay,*m_rtpPay,*m_rtpJitterBuffer; 
      GstElement* m_dtlssrtpDec;
      HmpCodecType m_codecType; 

      //functionalities
      gboolean construct();
      gboolean create_elements();
      gboolean add_and_link_elements();
      gboolean set_ptime(gint ptime, gint maxPtime);
};
/* HmpVoiceRePacketizeBin derived class - End
*/

/* HmpVoiceRelayBin derived class - Start
*/
class HmpVoiceRelayBin : public HmpBin
{
   public:
      //constructor and destuctor
      ~HmpVoiceRelayBin();
      HmpVoiceRelayBin(string binName, HmpBinType binType, gint recvPort, GSocket *recvSock);
      GstElement* m_dtlssrtpDec;

      //properties
      gboolean m_isSTTpipeline;

      //functionalities
      gboolean construct();
      gboolean add_and_link_elements();
};
/* HmpVoiceRelayBin derived class - End
*/

/* HmpWaveEncoderBin derived class - Start
*/
class HmpWaveEncoderBin : public HmpBin {
   public:
      //constructor and destuctor
      HmpWaveEncoderBin(string binName, HmpBinType binType, gint recvPort, GSocket *recvSocket);
      ~HmpWaveEncoderBin();

      //properties
      GstElement *m_rtpDepay,*m_audioDecoder,*m_wavEnc,*m_webrtcDsp;
      GstElement *m_fileSink,*m_capsFilter1,*m_reSampler1,*m_rtpjitterbuffer;
      GstElement *m_capsFilter2,*m_reSampler2,*m_alawenc,*m_alawdec;

      //functionalities
      gboolean construct();
      gboolean create_decode_elements();
      gboolean add_and_link_elements();
};
/* HmpWaveEncoderBin derived class - End
*/

/* HmpInhouseStreamingBin derived class - End
*/
class HmpInhouseStreamingBin : public HmpBin {
   public:
      //constructor and destuctor
      HmpInhouseStreamingBin(string binName, HmpBinType binType,HmpCodecType codecType,string codecName);
      ~HmpInhouseStreamingBin();

      //properties
      GstElement *m_audioConvert, *m_audioParse, *m_removeSilence,*m_capsfilter;
      GstElement *m_audioRate,*m_audioResample, *m_fdSrc, *m_payEnc, *m_rtpPay;
      HmpCodecType m_codecType;
      string m_codecName;
      gboolean m_in_pipeline;

      //functionalities
      gboolean construct();
      gboolean create_decode_elements();
      gboolean add_and_link_elements();
      gboolean set_ptime(gint ptime, gint maxPtime);
};
/* HmpInhouseStreamingBin derived class - End
*/

/* HmpVoiceTransceiverBin derived class - Start
*/
class HmpVoiceTransceiverBin : public HmpBin {
   public:
      //constructor and destuctor
      HmpVoiceTransceiverBin(string binName, HmpBinType binType, xGateASRType asrType, int recvPort, GSocket *sendSocket);
      ~HmpVoiceTransceiverBin();

      //properties
      xGateASRType m_asrType;
      GstElement  *m_rtpDepay, *m_rtpjitterbuffer, *m_audioDecoder;
      GstElement *m_audioConvert, *m_fdSink,*m_capSet,*m_reSampler,*m_rtpBin,*m_fakeSink,*m_id,*m_queue,*m_audioSplit;

      // functionalities
      gboolean construct();
      gboolean create_decode_elements();
      gboolean add_and_link_elements();   
      gboolean register_rtpbin_callback(gpointer data);
      static void on_pad_added(GstElement *rtpBin, GstPad *pad, gpointer data);
      static GstCaps * on_request_pt_map(GstElement *rtpBin, guint session, guint pt, gpointer data);
      gboolean register_appSink_callBack(gpointer);
      static gboolean on_setPayload(GstElement*,gpointer);
      gboolean m_in_pipeline;
      static void identity_callback(GstElement * identity,GstBuffer * buffer,gpointer data);
};
/* HmpVoiceTransceiverBin derived class - End
*/


/* HmpChannel bass class - Start
*/
class HmpChannel {
   public:
      //constructor and destructor
      HmpChannel(string VoiceMRCPPipelineName, HmpChannelType VoiceMRCPPipelineType);
      ~HmpChannel();

      //public properties
      string m_pipelineName;
      GstElement *m_pipeline;
      HmpChannelType m_pipelineType;
      GstBus *m_bus;
      guint *m_busId;
      GstStateChangeReturn m_pipeline_status;

      //public member functions
      gboolean register_handle_event_callback(gpointer data);
      static gboolean handle_event(GstBus* bus, GstMessage* msg, gpointer data);
      gboolean draw_dot_file();
      gboolean draw_pipeline_no_start();
      gboolean start();
      gboolean stop();
      gboolean pause();
      static gboolean reenable_voice_channels(Client*);
};
/*  HmpChannel bass class - End
*/


/* HmpVoiceTransceiverChannel derived class - Start
*/
class HmpVoiceTransceiverChannel : public HmpChannel {
   public:
      //constructor and destructor
      HmpVoiceTransceiverChannel(string pipelineName, HmpChannelType pipelineType);
      ~HmpVoiceTransceiverChannel();

      //member properties
      HmpVoiceTransceiverBin *m_pHmpVoiceTransceiverBin;

      //public member functions
      gboolean construct_bin(string binName, HmpBinType binType, xGateASRType asrType,\
	    gint audioPort, GSocket *audioRecvSock,guint pt);
      gboolean register_rtpbin_callback(gpointer data);
      gboolean register_appSink_callBack(gpointer);
};
/* HmpVoiceTransceiverChannel derived class - End
*/

/* HmpWaveEncoderChannel derived class - Start
*/
class HmpWaveEncoderChannel : public HmpChannel {
   public:
      //constructor and destructor
      HmpWaveEncoderChannel(string pipelineName, HmpChannelType pipelineType);
      ~HmpWaveEncoderChannel();

      //public member properties
      HmpWaveEncoderBin *m_pHmpWaveEncoderBin;

      //public member functions
      gboolean construct_bin(string binName, HmpBinType binType, \
	    gint audioPort, GSocket *audioRecvSock,guint pt);
};
/* HmpWaveEncoderChannel derived class - End
*/

/* HmpVoiceTranscodeChannel derived class - Start
*/
class HmpVoiceTranscodeChannel : public HmpChannel {
   public:
      //constructor and destructor
      HmpVoiceTranscodeChannel(string VoiceMRCPPipelineName, HmpChannelType VoiceMRCPPipelineType);
      ~HmpVoiceTranscodeChannel();

      //public member properties
      HmpVoiceRecvDecoderBin *m_pHmpVoiceRecvDecoderBin;
      HmpVoiceSendEncoderBin *m_pHmpVoiceSendEncoderBin;

      //public member functions
      gboolean construct_bin(string recvBinName, HmpBinType recvBinType,xGateTTSType ttsType,\
	    string sendBinName, HmpBinType sendBinType, HmpCodecType sendCodecType, \
	    int audioPort, GSocket *audioRecvSendSock);
      gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
      gboolean clear_client_addr();
      gboolean set_ptime(gint ptime, gint maxPtime);
      gboolean attach_audio_recv_send_bin();
      GSocket * get_recv_socket();
      void set_recv_socket(GSocket *socket);
};
/* HmpVoiceTranscodeChannel derived class - End
*/

/*  HmpVoiceRePacketizeChannel derived class - Start
*/
class HmpVoiceRePacketizeChannel : public HmpChannel
{
   public:
      //constructor and destructor
      HmpVoiceRePacketizeChannel(string pipelineName, HmpChannelType pipelineType);
      ~HmpVoiceRePacketizeChannel();

      //public member properties
      HmpVoiceRePacketizeBin *m_pHmpVoiceRePacketizeBin;

      //public member functions
      gboolean construct_bin(string binName, HmpBinType binType, HmpCodecType codecType, gint audioPort, GSocket *audioRecvSendSock);
      gboolean clear_client_addr();
      gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
      gboolean set_ptime(gint,gint);
};
/*  HmpVoiceRePacketizeChannel derived class - End
*/

/*  HmpVoiceRelayChannel derived class - End
*/
class HmpVoiceRelayChannel : public HmpChannel
{
   public:
      //constructor and destructor
      HmpVoiceRelayChannel(string pipelineName, HmpChannelType pipelineType);
      ~HmpVoiceRelayChannel();

      //public member properties
      HmpVoiceRelayBin *m_pHmpVoiceRelayBin;

      //public member functions
      gboolean construct_bin(string binName, HmpBinType binType, gint audioPort, GSocket *audioRecvSendSock);
      gboolean clear_client_addr();
      gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
};
/*  HmpVoiceRelayChannel derived class - End
*/

/* HmpInhouseStreamingChannel derived class - Start
*/
class HmpInhouseStreamingChannel : public HmpChannel {
   public:
      //constructor and destructor
      HmpInhouseStreamingChannel(string pipelineName, HmpChannelType pipelineType);
      ~HmpInhouseStreamingChannel();


      //public member functions
      gboolean construct_bin(string binName, HmpBinType binType,HmpCodecType codecType, string codecName);
      gboolean add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients = TRUE);
      gboolean set_ptime(gint,gint);

      //public member properties
      HmpInhouseStreamingBin *m_pHmpInhouseStreamingBin;
};

#endif
