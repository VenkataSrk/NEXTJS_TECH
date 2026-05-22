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
#include <gst/rtp/gstrtcpbuffer.h>
#include <gst/rtp/gstrtpbuffer.h>
}

//local include
#include "xGateUtil.h"
#include "xGateHmpGstUtil.h"
#include "xGateMgMsg.h"
#include "XGConfig.h"
#include "xGateHmpGstAudioBin.h"
#define HAVE_GST_DEBUG_PRINTS TRUE
#define MAX_VIDEO_RCV_STREAMS_PER_CONNECTION 6
#define SRC_PAD_NAME_LEN        50
#define MAX_DTLS_FAILED_RETRY_COUNT 3
#define RTP_STR(x) #x
#define SSRC_VAL ", ssrc=(uint)%u"
#define RANDOM_SSRC_ID g_random_int()

#define GENERIC_AUDIO_CAPS "application/x-rtp, clock-rate=(int)48000, media=(string)audio"
#define GENERIC_AUDIO_CAPS_PCM "application/x-rtp, clock-rate=(int)8000, media=(string)audio"
#define GENERIC_VIDEO_CAPS "application/x-rtp, clock-rate=(int)90000, media=(string)video"
#define UDP_CAPS_VID_VP8_x(x) "application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)VP8, payload=(int)96,ssrc=(uint)" #x //125
//#define UDP_CAPS_VID_VP8 "application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)VP8, payload=(int)96" //125
#define UDP_CAPS_VID_VP9 "application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)VP9, payload=(int)98"//127"
#define UDP_CAPS_VID_H264 "application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)H264, payload=(int)102, level-asymmetry-allowed=(string)1, packetization-mode=(string)1, profile-level-id=(string)42001f"//121"
#define UDP_CAPS_VID_H265 "application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)H265, payload=(int)123"//123"

#define UDP_CAPS_VID_H264_APPLE "application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)H264, payload=(int)96, level-asymmetry-allowed=(string)1, packetization-mode=(string)1, profile-level-id=(string)640c1f"//121"

#define STR(x) #x
#define RTP_CAPS_G722(x) "application/x-rtp, media=audio, encoding-name=G722, payload=(int)" STR(x)
#define RTP_CAPS_OPUS(x) "application/x-rtp, media=audio, encoding-name=OPUS, encoding-params=(string)2, payload=(int)" STR(x)
#define RTP_CAPS_PCMU(x) "application/x-rtp, media=audio, encoding-name=PCMU, payload=(int)" STR(x)
#define RTP_CAPS_VP8(x) "application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)VP8, payload=(int)" STR(x)
#define RTP_CAPS_VP9(x) "application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)VP9, payload=(int)" STR(x)
#define RTP_CAPS_H264(x) "application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)H264, payload=(int)" STR(x)
#define RTP_CAPS_H265(x) "application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)H265, payload=(int)" STR(x)

#define UDP_SIMULCAST_CAPS_VID_VP8 "application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)VP8, payload=(int)100" //96

#define SIMULCAST_CAPS_VID_VP8 "application/x-rtp, clock-rate=(int)90000, media=(string)video, encoding-name=(string)VP8, payload=(int)96, simulcast=(string)max-fps" //96

#define UDP_CAPS_VID_VP8 UDP_SIMULCAST_CAPS_VID_VP8
#define APPSRC_AUDIO_MAX_BUFFERS 20
#define AUDIO_MCU_ENABLED
#define REUSE_MLINE_VIDEO_TX
#define AUDIO_MCU_MID_LINE 2
#define SCREENSHARE_MID_LINE 3
#define MEDIA_CAPS_FOR_AUDIO 0
#define MEDIA_CAPS_FOR_VIDEO 1
#define MEDIA_CAPS_FOR_AUDIOVIDEO 2
#define LOG_DEBUG_PRINT_NUM_PACKET 1000
/* jitter buffer config in milli sec */
#define JITTER_BUFFER_VAL    2000
#define IS_EMPTY_SDP_ATTRIBUTE(val) (val == NULL || g_strcmp0(val, "") == 0)

//#define SIMULCAST_ENABLE
//#define USE_VP8_CODEC_CONF
#define USE_VP9_CODEC_CONF

#ifdef SIMULCAST_ENABLE
#define USE_VP8_CODEC_CONF
#endif

#define ENABLE_TWCC_TX_PATH
#define ENABLE_TWCC_RX_PATH
//#define ENABLE_VIDEO_PLAYOUT_DELAY
//#define LOG_RTP_SEQNUM_EVERY_VIDEO_PKT
#define VIDEO_PLAYOUT_DELAY_EXTMAP_STR "http://www.webrtc.org/experiments/rtp-hdrext/playout-delay"
#define TWCC_EXTMAP_STR "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01"
#define MAX_BTRT 131071
#define AUDIO_LEVEL_EXTMAP_STR "urn:ietf:params:rtp-hdrext:ssrc-audio-level"
#define VIDEO_MIN_PLAYOUT_DELAY_MS 40
#define VIDEO_MAX_PLAYOUT_DELAY_MS 100
#define MAX_RTP_SEQNUM 65535
#define MICROSEC_SLEEP_400    400000
#define RECORD_ENABLED //yoga & team defined this for recording feature

#define CAPS_STR_FOR_VIDEO "application/x-rtp"
#define CAPS_STR_FOR_AUDIODEC "audio/x-raw, format=(string)S16LE, rate=(int)16000, channels=(int)1, layout=(string)interleaved"
#define CAPS_STR_FOR_AUDIOENC "audio/x-raw, format=(string)S16LE, rate=(int)16000, channels=(int)1, layout=(string)interleaved"
#define NICESRC_ELEMENT_NAME "nicesrc"
#define NICESINK_ELEMENT_NAME "nicesink"
#define NICESRC_ELEMENT_PRIORITY  10
#define VIDEO_SAMPLES_1SEC    90000
#define AUDIO_SAMPLES_1SEC    48000
#define NANO_TO_MILLI_SEC     1000000
#define MILLI_TO_SEC          1000
#define DTX_TIME_DIFF_MS      300
#define AUDIO_SAMPLE_TIME_OFFSET_20MS 960
#define BYTES_PER_SAMPLE_16BIT 2
#define AUDIO_SAMPLES_16K_20MS 320
#define CUTOFF_AUDIO_LEVEL_TO_DECODE 60
#define MAX_LATENCY_MS               100
#define LAST_AUDIO_MIXED_TIME_DIFF_MS 100
//unsigned char* VIDEO_CAPS[] = {UDP_CAPS_VID_H264,UDP_CAPS_VID_VP8,UDP_CAPS_VID_VP9,UDP_CAPS_VID_H265};
unsigned char* VIDEO_CAPS[] = {UDP_SIMULCAST_CAPS_VID_VP8};
//unsigned char* VIDEO_CAPS[] = {UDP_CAPS_VID_H264_APPLE};
//unsigned char* VIDEO_CAPS[] = {UDP_CAPS_VID_H264};

typedef enum {
  STREAM_START_EVENT_PAD = 0,
  STREAM_START_EVENT_ELEMENT,
  SEGMENT_EVENT_TIME_PAD,
  SEGMENT_EVENT_BYTES_PAD,
  CAPS_EVENT_PAD,
  CAPS_EVENT_ELEMENT
}MgGstEventType;

typedef enum {
 GENERATE_OFFER = 0,
 GENERATE_ANSWER,
 CONFIGURE_LOCAL_DESCRIPTION,
 CONFIGURE_REMOTE_DESCRIPTION,
};

typedef enum {
  RTP_HDR_EXTN_TWCC = 0,
  RTP_HDR_EXTN_AUDIO_LEVEL,
  RTP_HDR_EXTN_VIDEO_PLAYOUT_DELAY,
  RTP_HDR_EXTN_MAX
} HDR_EXTN_TYPE;
#ifdef RECORD_ENABLED
typedef enum {
  RECORD_STATE_NULL=0,
  RECORD_STATE_START=1,
  RECORD_STATE_PAUSE=2,
}eRecState;
#endif

typedef enum {
  SINK_PAD_TYPE_UNKNOWN=0,
  SINK_PAD_TYPE_AUDIO=1,
  SINK_PAD_TYPE_VIDEO=2,
}ePadType;

typedef enum {
  STREAM_TYPE_UNKNOWN=0,
  STREAM_TYPE_AUDIO=1,
  STREAM_TYPE_VIDEO=2,
  STREAM_TYPE_SCREENSHARE=3
}StreamType;

typedef struct _TWCCStats {
  guint bitrate_sent;
  guint bitrate_recv;
  guint packets_sent;
  guint packets_recv;
  gfloat packet_loss_pct;
  GstClockTimeDiff avg_delta_of_delta;
  gfloat avg_delta_of_delta_change;
}TWCCStats;

typedef struct _RTPRecvReportStats{
  guint   uiLastRRFractLost;
  guint   uiLastRRPacketLost;
  guint   uiLastRRExtendedSeqNum;
  guint   uiLastSR;
  guint   uiDelayLastSR;
  guint   uiInterArrivalJitter;
  guint   uiRTT;
}RTPRecvReportStats;

typedef enum _LAYER_ID {
  LAYER_ID_UNKNOWN = -1,
  LAYER_ID_0 = 0,
  LAYER_ID_1,
  LAYER_ID_2,
  LAYER_ID_MAX,
}LAYER_ID;

typedef struct _ImageInfo {
  unsigned short u16Width;
  unsigned short u16Height;
} ImageInfo;

typedef struct _Scalability_Struct_Info {
  ImageInfo asImageInfo [LAYER_ID_MAX];
  unsigned  char ucNumSpatialLayers;
} Scalability_Struct_Info;

typedef struct _SvcInfo {
  Scalability_Struct_Info sScalabilitStructInfo;
  unsigned  char ucCurrentSpatialLayerID;
  unsigned  char ucTargetSpatialLayerID;
  GstBufferList  *pPrevLayerBufferList;
  GstBufferList  *pNewLayerBufferList;
  gboolean       bKeyFrameAllLayersSent;
}SVCInfo;


class Client;
// class that stores SSRC, payload, seq_num
class offerDetail {
public:
   unsigned int uiTxSSRCNum;
   unsigned int uiRTXTxSSRCNum;
   unsigned short usSeqNum;
   unsigned short usLastTxSeqNum;
   unsigned short usRTXSeqNum;
   unsigned short usPrevRcvdSeqNum;
   guint32 uiPrevRTPTimestamp;
   GstClockTime lastClockTime;
   guint32 uiTxTimestamp;
   int uiPayloadNum;
   int uiRTXPayloadNum;
   string MediaLineIdx;
   string  sinkPadName;
   GstPad  *sinkPad;
   ePadType padType;
   GstElement  *pPeerElement;
   GstElement  *m_appsrc;
   gboolean bTileChanged;
   gboolean    bSendPkt;
   gchar       encoding_name[50];
   unsigned long ulNumPktSent;
   GstBufferList  *pOldBufferList;
   GstBufferList  *pNewBufferList;
   SVCInfo         sSvcInfo;
   RTPRecvReportStats sPrevRTPStats;
   RTPRecvReportStats sCurrentRTPStats;
   offerDetail():bTileChanged(FALSE),
                 padType(SINK_PAD_TYPE_UNKNOWN)
   {
   }
   ~offerDetail(){}
};
// Client offer details for audio and video media
class mlineofferDetail : public ACE_Task<ACE_MT_SYNCH>
{
   public:
    offerDetail audioOfferDetails;
    offerDetail videoOfferDetails;
    Client      *pClient;
    string      callId_offer_generated;
    gboolean    bPopulated;
    xGateJoinerType eJoinerType;
    ~mlineofferDetail (){
       if (callId_offer_generated.size())
         callId_offer_generated.clear();
       if (audioOfferDetails.sinkPadName.size())
         audioOfferDetails.sinkPadName.clear();
       if (videoOfferDetails.sinkPadName.size())
         videoOfferDetails.sinkPadName.clear();
       pClient = NULL;
       bPopulated = FALSE;
    }
};

typedef std::unordered_map<string, mlineofferDetail*> MLINE_OFFER_MAP;
typedef vector <mlineofferDetail*> MLINE_OFFER_VECTOR;
/* HmpVideoBin Base class - Start
 */
class HmpVideoBin {
  public:
    /*constructor and destuctor */
    HmpVideoBin(string binName, HmpVideoBinType binType, unsigned int m_dynamicCodec);
    ~HmpVideoBin();

    /*properties */
    GstBin *m_bin;
    string m_binName;
    HmpVideoBinType m_vbinType;
    unsigned int m_dynamicCodec;
    /*functionalities */
    virtual gboolean construct(string webrtc_name) = 0;
    gboolean send_stream_start_event (void *obj, MgGstEventType eEvent, gchar *u8StreamId);
    gboolean send_segment_event (GstPad *sinkPad, MgGstEventType eEvent);

    gboolean draw_dot_file();
};
/* HmpVideoBin Base class - End
 */

/* HmpFrameBin derived class - Start
*/
class HmpFrameBin : public HmpVideoBin {
  public:
    //constructor and destuctor
    HmpFrameBin(string binName, HmpVideoBinType binType, HmpCodecType codecType,
                                unsigned int dynamicCodec,
		                string sdpStr, HmpCallDir callDir,
                                xGateWebRtcBinType webRtcType, string webrtc_name1);
    ~HmpFrameBin();

    //properties
    HmpCodecType m_codecType;
    unsigned int m_dynamicCodec;
    GstElement *m_webrtcbin;
    string m_remoteSdpStr;
    string m_localSdpStr;
    /* Values stored are used when sending message type 10 */
    string d_rtp_ep;
    string o_rtp_ep;
    string m_webrtc_name;
    int joiner_type;
    gboolean is_dtls_failed;
    GObject  *gst_rtp_session;
    GObject  *rtp_session;
    TWCCStats  sPrevTwccStats;
    /* End of Values stored are used when sending message type 10 */

    int m_rtp_stream_ext_id;
    int m_repair_rtp_stream_ext_id;
    int m_negotiated_audio_twcc_ext_Id;
    int m_negotiated_audio_level_Id;
    int m_negotiated_video_twcc_ext_Id;
    int m_negotiated_video_delay_playout_ext_Id;
    unsigned int rtp_video_rx_ssrc;
    unsigned int rtcp_sender_ssrc;
    string m_mid_video;
    GstWebRTCSDPType m_remoteSdpType;
    GstWebRTCBundlePolicy m_bundlePolicy;
    GstWebRTCSDPType m_sdpType;
    HmpCallDir m_callDir;
    gboolean m_isLocalSdpCreated;
    gboolean bStartedRecvAudio;
    guint ssrc_audio;
    guint ssrc_video;
    guint retrieved_audiossrc;
    guint retrieved_videossrc;
    xGateMgMsgType eResponseMsgType;
    xGateWebRtcBinType m_webRtcBinType;
    map<guint32,string> m_rtcp_stream_id;
    map<guint32,string> m_rtcp_repair_stream_id;
    map<string,gint> m_codec_payload_map;
    string AudioEncodingParam;
    string VideoEncodingParam_caps;
    GstPad   *audioSrcPad;
    GstPad   *videoSrcPad;
    string   audioSrcPadName;
    string   videoSrcPadName;
    gint m_negotiated_audio_payload_num;
    char m_negotiated_audio_codec[10];
    gint m_negotiated_video_payload_num;
    char m_negotiated_video_codec[10];
    unsigned long m_ulNumAudioPktRcvd;
    unsigned long m_ulNumAudioPktSent;
    unsigned long m_ulNumVideoPktRcvd;
    unsigned long m_ulNumVideoPktSent;
    //functionalities
    gboolean construct(string webrtc_name);
    gboolean create_webrtcbin(string webrtc_name);
    gboolean register_webrtcbin_callback(gpointer data);
    gboolean handle_sinkpad_event (GstPad *srcPad, Client *client);
    gboolean handle_srcpad_event (GstPad *srcPad, Client *client);
    gboolean add_audiomixer_sinkpad(Client *client,Client *src_client, guint uiSSRC);
    //rtpbin element callback functions
    static void handle_pad_added_callback(GstElement *rtpBin, GstPad *pad, gpointer data);
    static void on_rtcp_received(GstElement *rtpBin, GstBuffer *pBuffer, gpointer data);
    static gboolean on_rtcp_send(GstElement *rtpBin, GstBuffer *pBuffer, gboolean early_rtcp, gpointer data);
    void get_twcc_stats (HmpFrameBin *pFrameBin, TWCCStats *pTwccStat);
    static void on_pad_removed_vid(GstElement *rtpBin, GstPad *pad, gpointer data);
    static void on_new_data_rcvd_srcPad(Client *client, gchar *element_name, void *list);
    void handle_data_rcvd_rx_thread (Client *src_client, gchar *element_name,
                                   GstBuffer *buffer, void *vpParam);
    static GstFlowReturn handle_data_rcvd_srcpad (GstPad * pad, GstObject * parent, GstBufferList * list);
    static GstPadProbeReturn handle_PadProbeCallback (GstPad * pad, GstPadProbeInfo * info, gpointer user_data);
    static GstPadProbeReturn handle_audioRTPPadProbeCallback(GstPad * pad, GstPadProbeInfo * info, gpointer user_data);
    static void SendRTPPktToDstClient (Client *client, gchar *element_name,
		                      GstBuffer *buffer, HmpFrameBin *src_frameRecvBin,
                                      void *mlineDetail);
    static xGateRetVal PostBufferToThread (Client *dst_client, gchar *element_name,
                                    void *buffer, gboolean bIsList, void *mlineDetails);
    GstBufferList *HmpFrameBin::Modify_RTP_Header (gchar *element_name,GstBuffer *buffer,
                                       offerDetail *pOfferDetails, gboolean bIsBufferFromList);
    GstWebRTCSessionDescription* create_webrtc_session_description(GstWebRTCSDPType sdpType, string &sdpStr);
    void extract_extension_id(GstWebRTCSessionDescription*);
    int get_extension_id(gchar*);
    void AddPlayOutDelayExtension (HmpFrameBin *pFrameBin, GstRTPBuffer *pGstBuffer);
    gboolean Detect_voice_activity (Client *client, void *vBuffObj);
    gboolean Check_Transceiver (Client *client, string &remoteSdpStr, guint );
    gboolean get_rtp_stream_info(GstRTCPPacket*,Client*);
    gboolean handle_sdp_offer(gpointer udata);
    gboolean handle_sdp_answer(gpointer udata);

    //webrtcbin element callback functions
    static void on_negotiation_needed_cb(GstElement *webrtcbin, gpointer udata);
    static gboolean configure_sdp_to_generate_offerOrAnswer (Client *client, void *pFrameChannel,
                                                                    string &SdpStr, string callId, int sdpType);
    static void on_offer_created_cb(GstPromise * promise, gpointer udata);
    static void on_answer_created_cb(GstPromise * promise, gpointer udata);
    static void on_ice_gathering_state_change (GstElement *webrtcbin , GParamSpec * pspec, gpointer data);
    static void on_connection_state_change (GstElement *webrtcbin , GParamSpec * pspec, gpointer data);
    static void on_ice_connection_state_change (GstElement *webrtcbin , GParamSpec * pspec, gpointer data);
    static void send_key_frame_request_to_mbc (Client *client, string callId);
    static void handle_ice_candidate_msg (HmpFrameBin * HmpFrameBin,
                                         xGateWebRtcBinType webRtcBinType, Client *pClient);
    static void handle_connection_failed_msg (HmpFrameBin * HmpFrameBin,
                                             xGateWebRtcBinType webRtcBinType, Client *pClient);
    static void handle_send_updated_sdp_msg (HmpFrameBin *pHmpFrameBin, Client *pClient);
    GstCaps *generate_MediaServer_caps(Client* pClient, guint media_type, guint transceiver_direction);
    static void modify_sdp_params(HmpFrameBin * HmpFrameBin,GstWebRTCSessionDescription *offer);
    const gchar *media_get_ice_ufrag (const GstSDPMessage * msg, guint media_idx);
    const gchar *media_get_ice_pwd (const GstSDPMessage * msg, guint media_idx);
    const gchar *media_get_fingerprint (const GstSDPMessage * msg, guint media_idx);
    void get_sdp_attributes(const GstSDPMessage * sdp, char **ice_ufrag, char **ice_pwd, char **fingerprint);
    guint media_get_ssrc(guint media_idx);
    void DecodeRTCPPacket(GstRTCPPacket *rtcp_packet,HmpFrameBin *pFrameBin, Client *client);
    void DecodeRTCPPSFBPacket(GstRTCPPacket *rtcp_packet, HmpFrameBin *pFrameBin, Client *client);
    void DecodeRTCPTransportFeedBack(GstRTCPPacket *rtcp_packet, HmpFrameBin *pFrameBin, Client *client);
    void SendKeyFrameRequest(Client *client);
    static void RegisterRTCPCallback(void *data);
    static gint media_get_payload (string sdp_str, char *codec_name, char *encoding_param);
    static string get_mline_index (string sdp_str, int pos);
    void HmpFrameBin::generate_mline_sdp_update_request (Client *pClient,
                                               PARTICIPANT_LIST *client_list,string remote_sdp);
    void HmpFrameBin::Check_Inactivate_mline (Client *pClient,
                                               PARTICIPANT_LIST *client_list,string remote_sdp);

    static void populate_offer_details (Client *client, string sdp_str, mlineofferDetail *offerDetails);
    //RTP Header ext related
    gboolean write_rtp_header_extn_data(GstBuffer**,GstBuffer*,Client*,GstMemory*,GstMemory*,guint8,guint8,guint8*,
		                        guint8*,guint8,guint8);
   gboolean modify_rtp_header_extn_data(GstBuffer*,guint8,guint8,guint8*,guint8*,guint8,guint8,Client*);
   gboolean read_rtp_header_extn_data(GstBuffer *buffer,int *size,guint8 *data,guint8 extId,Client*);
   static GstBuffer* check_modify_rtp_hdr_extn (Client* src_client, Client* dst_client,
		                                GstBuffer* buffer, gchar* element_name,
						HmpFrameBin *src_frameRecvBin, HmpFrameBin *dst_frameRecvBin);
   gboolean fill_fixed_header(GstRTPBuffer *rtp,GstMemory **header_mem);
   void fill_payload(GstRTPBuffer *rtp,GstMemory **payload_mem);
   gboolean write_rtp_header_data(GstMemory **header_mem,GstMemory **payload_mem,GstBuffer *buffer,guint32 ssrc);
   GstMemory* copy_ext_mem(guint8* ext_mem,guint num_bytes);
   gint extract_ext_id (string remote_sdp, guint media_type, HDR_EXTN_TYPE eHdrExtntype);
   GstBuffer* make_new_buffer(GstBuffer *src,GstMemory *hdr_mem,GstMemory *ext_mem,
		                            GstMemory *payload_mem);
   string Inactivate_mline_SDP (string sdp_str, offerDetail *offer, xGateSDPType sdpType);
   static xGateSDPType getSDPType (string sdp);
   gboolean AddTransceivers (Client *client,ParticipantInfo sPartInfo,
                             string remoteSdpStr,int offer_answer, guint transceiver_direction);
   unsigned int tmmbr_calculate(unsigned int bitrate);
   static guint32 retrieve_ssrc_sdp (string sdp, unsigned int mediatype);

#ifdef RECORD_ENABLED
   static gboolean push_to_rcvd_media_handler(gchar *element_name, gpointer buffer, Client *client);
#endif

};
/* HmpFrameBin derived class - End
*/

/* HmpAudioDecoderBin derived class - Start
*/
class HmpAudioDecoderBin : public HmpVideoBin {
  public:
    //constructor and destuctor
    HmpAudioDecoderBin(string binName, HmpVideoBinType binType, HmpCodecType codecType,string decoder_name);
    ~HmpAudioDecoderBin();

    HmpCodecType m_audioCodecType;
    gboolean construct(string decoder_name);
    gboolean create_decoderbin(string decoder_name);
    string m_decoder_name;
    GstPad *m_ghostRtpSinkPad= NULL;
    GstElement *m_opusdepay = NULL;
    GstElement *m_opusdec = NULL;
    GstElement *m_opusdec_queue = NULL;
    GstElement *m_audioresample = NULL;
    GstElement *m_capsfilter = NULL;
    GstElement *m_audioconvert = NULL;
    GstElement *m_audioLevel = NULL;
};
/* HmpAudioDecoderBin derived class - End
*/
class AUDIO_MIXER_INPUT_INFO {
public:
  GstPad    *sink_pad;
  GstElement *m_appsrc;
  GstElement *pPeerElement;
  string    src_callId;
  gboolean   bStartEventSent;
};
typedef vector <AUDIO_MIXER_INPUT_INFO *> AUDIO_MIXER_INPUT_VECTOR;

/* HmpAudioEncoderBin derived class - Start
*/
class HmpAudioEncoderBin : public HmpVideoBin {
  public:
    //constructor and destuctor
    HmpAudioEncoderBin(string binName, HmpVideoBinType binType, HmpCodecType codecType,string encoder_name);
    ~HmpAudioEncoderBin();

    HmpCodecType m_audioCodecType;
    string m_encoder_name;
    gboolean bSetMarkbit;
    gboolean bDtxEnabled;
    GstClockTime  lastAudioMixedTime;
    GstPad *m_ghostRtpSrcPad= NULL;
    GstElement *m_audiomixer = NULL;
    GstElement *m_opuspay = NULL;
    GstElement *m_opusenc = NULL;
    GstElement *m_opusenc_queue = NULL;
    GstElement *m_opusenc_filter = NULL;
    GstElement *m_audioresampler = NULL;
    GstElement *m_audioconvert = NULL;
    AUDIO_MIXER_INPUT_VECTOR m_mixer_input_info;
    GMutex     m_mixer_lock;
    gboolean construct(string webrtc_name) override {};
    gboolean create_encoderbin(string encoder_name, Client *client);
    static GstPadProbeReturn handle_EncodedAudioProbeCallback (GstPad * pad,
                         GstPadProbeInfo * info, gpointer user_data);
    static GstPadProbeReturn handle_GstAudioMixerProbeCallback (GstPad * pad,
                         GstPadProbeInfo * info, gpointer user_data);
    void check_update_rtp_timestamp (GstRTPBuffer     *rtpbuffer,
                                      mlineofferDetail *pmlineDetail,
                                      Client           *client,
                                      GstClockTime     clocktime);
    GstPad *Generate_new_Audiomixer_sinkpad();
    static void remove_audiomixer_sinkpad(Client *pClient, string &callId);
    gboolean check_dtx_enabled (string &sdp);

};
/* HmpAudioEncoderBin derived class - End
*/
#ifdef RECORD_ENABLED
/* HmpRecordingSendBin derived class - Start
*/
class HmpRecordingSendBin {
  public:
    //constructor and destuctor
    HmpRecordingSendBin(string binName);
    ~HmpRecordingSendBin();

    GstBin *m_bin;
    string m_binName;
    GSocket *m_sendSocket;
    gint m_udpSrcPort;
    gint m_udpDstPort;
    string m_sendIp;
    gchar *m_streamId;
    GstElement *m_udpsink;
    GstPad *m_udpSinkPad;

    gboolean create_recording_send_bin(char *element_name);
    gboolean connect_send_socket();
    gboolean send_start_event();
    gboolean draw_dot_file();
};
/* HmpRecordingSendBin derived class - End
*/
#endif
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
    static gboolean handle_bus_event(GstBus* bus, GstMessage* msg, gpointer data);
    static void handle_stream_status (GstMessage* msg, gpointer data);
    gboolean draw_dot_file();
    gboolean start();
    gboolean stop();
};
/*  HmpChannel bass class - End
*/

/* HmpFrameChannel derived class - Start
*/
class HmpFrameChannel : public HmpVideoChannel {
  public:
    HmpFrameChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpFrameChannel();

   //public member functions
    gboolean construct_bin(string binName, HmpVideoBinType vbinType, HmpCodecType codecType, unsigned int dynamicCodec, string sdpStr, HmpCallDir callDir, xGateWebRtcBinType webRtcType, string webrtc_name, Client *client);
    gboolean register_webrtcbin_callback(gpointer data);
    gboolean handle_sdp_offer(gpointer udata);
    gboolean handle_sdp_answer(gpointer udata);
    gboolean construct_encoder_bin (HmpVideoBinType vbinType,
                                    HmpCodecType codecType, Client *client);
    gboolean construct_decoder_bin (HmpVideoBinType vbinType,
                                    HmpCodecType codecType, Client *client);

   //public member properties
    HmpFrameBin *m_pHmpFrameBin;
    HmpAudioDecoderBin *m_pHmpAudioDecoderBin;
    HmpAudioEncoderBin *m_pHmpAudioEncoderBin;
};
/*  HmpFrameChannel derived class - End
*/


#ifdef RECORD_ENABLED
/* HmpRecordVideoSendChannel derived class - Start
*/
class HmpRecordVideoSendChannel : public HmpVideoChannel {
  public:
    HmpRecordVideoSendChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpRecordVideoSendChannel();

    gboolean construct_channel(Client *client);

    //public properties
    HmpRecordingSendBin *m_pHmpRecVideoSendBin = NULL;
    gboolean m_start;
};
/* HmpRecordVideoSendChannel derived class - End
 */

/* HmpRecordAudioSendChannel derived class - Start
*/
class HmpRecordAudioSendChannel : public HmpVideoChannel {
  public:
    HmpRecordAudioSendChannel(string pipelineName, HmpChannelType pipelineType);
    ~HmpRecordAudioSendChannel();

    gboolean construct_channel(Client *client);

    //public properties
    HmpRecordingSendBin *m_pHmpRecAudioSendBin = NULL;
    gboolean m_start;
};
/* HmpRecordAudioSendChannel derived class - End
 */

/* HmpRecordSendChannel derived class - Start
*/
class HmpRecordSendChannel {
  public:
    HmpRecordSendChannel();
    ~HmpRecordSendChannel();

    gboolean construct_record_send_channel(Client *client);
    gboolean stop_record();
    gboolean start_record();

    void handle_record_start_res(Client *client);
    void handle_record_resume_res(Client *client);
    void generate_key_frame_req(Client *client);
    void handle_send_recording_msg(Client *client);

    //public properties
    HmpRecordAudioSendChannel *m_audioSendChannel;
    HmpRecordVideoSendChannel *m_videoSendChannel;
    string m_clientId;
    eRecState m_recState;
};
/*  HmpRecordingSendChannel derived class - End
*/
#endif
#endif
