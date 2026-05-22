#ifndef _XGATE_HMP_GST_UTIL_H
#define _XGATE_HMP_GST_UTIL_H

//gstreamer includes
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

//local includes
#include "xGateUtil.h"
#include "xGateAuditMsg.h"

#define GST_DEBUG TRUE
#define LOOPBACK_IP "127.0.0.1"
#define CONNECTSTR "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define CONNECTSTR_LEN strlen(CONNECTSTR)

//Enum declaration - start
typedef enum
{
   EN_RTP_PORT_UNKNOWN,
   EN_RTP_PORT_AUDIO,
   EN_RTP_PORT_VEDIO,
   EN_RTP_PORT_LOOPBACK,
} HmpRtpPortType;

typedef enum
{
   EN_CODEC_UNKNOWN = -1,
   EN_CODEC_PCMU = 0,
   EN_CODEC_PCMA = 8,
   EN_CODEC_G722 = 9,
   EN_CODEC_CN = 13,
   EN_CODEC_G729 = 18,
   EN_RTP_DTMF_EVENT = 101,
   EN_CODEC_OPUS = 111,
   EN_CODEC_H264 = 121,
   EN_CODEC_H265 = 123,
   EN_CODEC_VP8 = 125,
   EN_CODEC_VP9 = 127,
} HmpCodecType;

typedef enum
{
   EN_MEDIA_UNKNOWN,
   EN_MEDIA_RTP,
   EN_MEDIA_SRTP,
   EN_MEDIA_DTLS,
} HmpMediaMode;

typedef enum
{
   EN_CALL_DIR_UNKNOWN = 0,
   EN_CALL_DIR_IN = 1,
   EN_CALL_DIR_OUT = 2,
} HmpCallDir;

typedef enum
{
   EN_CALL_STATE_UNKNOWN = 0,
   EN_CALL_STATE_IN_PROGRESS = 1,
   EN_CALL_STATE_CONNECTED = 2,
   EN_CALL_STATE_DISCONNECTED = 3,
} HmpCallState;

typedef enum 
{
   EN_BIN_PRESENT_UNKNOWN=0,
   EN_BIN_PRESENT_RECV_END,
   EN_BIN_PRESENT_SEND_END

}BinPresentState;
typedef enum
{
   EN_PIPELINE_UNKNOWN,
   EN_PIPELINE_VOICE_TRANSCODE,
   EN_PIPELINE_VOICE_REPACKETIZE,
   EN_PIPELINE_VOICE_RELAY,
   EN_PIPELINE_VOICE_CAPTURE,
   EN_PIPELINE_WAVE_ENCODER,
   EN_PIPELINE_VOICE_STREAMING,
   EN_PIPELINE_INHOUSE_STREAM,
   EN_PIPELINE_VOICE_SEND
} HmpChannelType;

typedef enum
{
   EN_BIN_UNKNOWN,
   EN_BIN_G711U_SEND,
   EN_BIN_G711U_RECV,
   EN_BIN_G711A_SEND,
   EN_BIN_G711A_RECV,
   EN_BIN_G722_SEND,
   EN_BIN_G722_RECV,
   EN_BIN_G729_SEND,
   EN_BIN_G729_RECV,
   EN_BIN_OPUS_RECV,
   EN_BIN_OPUS_SEND,
   EN_BIN_DTLS_RECV,
   EN_BIN_DTLS_SEND,
   EN_BIN_DTLS_SEND_G711A,
   EN_BIN_DTLS_SEND_G711U,
   EN_BIN_DTLS_SEND_G722,
   EN_BIN_DTLS_SEND_OPUS,
   EN_BIN_DTLS_SEND_G729,

} HmpBinType;


struct ClientDetail {
   string m_mgresourceId;
   xGateCallType m_callType;
   string m_callId;
   string m_group_callId;
   string m_switch_callId;
   unsigned int m_gateway_id;
   unsigned int m_asrType;
   unsigned int m_ttsType;
   unsigned int m_is_ccaas;
   unsigned int m_ccaas_port_type;
   int m_legId;
   int m_context_id;
   HmpMediaMode m_mediaModeIn;
   HmpMediaMode m_mediaModeOut;
   string m_dtmfBuffer;
   xGateMediaEvent m_mediaEvent;
   xGateJoinerType m_joinerType;
   unsigned int m_reset_flag;
   HmpCallDir m_callDir;
   unsigned int m_sigType;
   string m_rtpEp;
   AudioDetail audioDetail;
   string m_filePath;
   long int m_fileSize;
   string m_pbxIp;
   unsigned int m_pbxPort;
   unsigned int m_ptime;
   unsigned int m_maxPtime;
   gboolean m_adHoc;
   unsigned int m_dtmfPt;
   unsigned int m_mrcp_port_allocate;
   unsigned int m_mrcp_port_activate;
   string m_mrcp_ip_info;
   unsigned int m_mrcp_port;
   unsigned int m_mrcp_ttsport;
   string m_mrcp_codec_name;
   unsigned int m_mrcp_codec;
   unsigned int m_mrcp_ptime;
   string m_mrcp_mg_ip;
   string m_mrcp_mg_port;
   string m_mrcp_server_ip;
   string m_mrcp_server_port;
   string m_mrcp_ttsServer_ip;
   string m_mrcp_ttsServer_port;
   string m_mrcpUid;
   string m_mrcpCallid;
   string m_mrcpDomainid;
   string m_mrcp_Devicetype;
   string m_mrcp_Ext;
   unsigned int  m_mrcp_isSttEnabled;
   unsigned int m_transcript_mode;
   unsigned int m_client_port; 
   unsigned int m_tts_enabled;
   unsigned int  m_mg_port;
   string m_mg_Ip;
   unsigned int m_audioTtsPort;
   unsigned int m_audioPort;
   string m_transExt;
   string m_ccaas_block_id;
   string m_ccaas_block_type;
   string m_ccaas_channel_id;
   unsigned int m_tts_fd;
   unsigned int m_ccaas_voice_recog_port;
   string m_ccaas_agent_id;
   string m_ccaas_bot_url;

   ClientDetail() : 
      m_mgresourceId(""), m_callType(EN_XGATE_CALL_TYPE_UNKNOWN), m_callId(""), m_group_callId(""), m_switch_callId(""), m_gateway_id(0), m_legId(-1), 
      m_context_id(0), m_mediaModeIn(EN_MEDIA_UNKNOWN), m_mediaModeOut(EN_MEDIA_UNKNOWN), m_dtmfBuffer(""),
      m_mediaEvent(EN_MEDIA_EVENT_NULL), m_joinerType(EN_JOINER_TYPE_UNKNOWN), m_callDir(EN_CALL_DIR_UNKNOWN),
      m_sigType(3), m_rtpEp(""), m_filePath(""), m_fileSize(0), m_pbxIp(""), m_pbxPort(0), m_ptime(0),
      m_maxPtime(0),m_adHoc(FALSE),m_reset_flag(0),m_dtmfPt(101),m_mrcp_port_allocate(0),m_mrcp_port_activate(0),m_mrcp_ip_info(""),m_mrcp_port(""),m_asrType(0),m_is_ccaas(0),m_ccaas_port_type(0),m_mrcp_codec_name(""),m_mrcp_codec(0),m_mrcp_ptime(0),m_mrcp_mg_port(""),m_mrcp_mg_ip(""),m_mrcp_server_port(""),m_mrcp_server_ip(""),m_ccaas_block_id(""),m_ccaas_block_type(""),m_ccaas_channel_id(""),m_ccaas_agent_id(""),
      m_mrcpUid(""),m_mrcpCallid(""),m_mrcpDomainid(""),m_mrcp_Devicetype(""),m_mrcp_Ext(""),m_mrcp_isSttEnabled(0),m_transcript_mode(0),m_tts_enabled(0),m_mg_Ip(""),m_mg_port(0),m_audioTtsPort(0),m_audioPort(0),m_transExt(""),m_tts_fd(0),m_ccaas_voice_recog_port(0),m_ttsType(0),m_ccaas_bot_url("")
   {};

   ClientDetail& operator=(ClientDetail &src)
   {
      m_mgresourceId = src.m_mgresourceId;
      m_callType = src.m_callType;
      m_asrType = src.m_asrType;
      m_ttsType = src.m_ttsType;
      m_is_ccaas = src.m_is_ccaas;
      m_ccaas_port_type = src.m_ccaas_port_type;
      m_callId = src.m_callId;
      m_group_callId = src.m_group_callId;
      m_switch_callId = src.m_switch_callId;
      m_gateway_id = src.m_gateway_id;
      m_legId = src.m_legId;
      m_context_id = src.m_context_id;
      m_mediaModeIn = src.m_mediaModeIn;
      m_mediaModeOut = src.m_mediaModeOut;
      m_dtmfBuffer = src.m_dtmfBuffer;
      m_joinerType = src.m_joinerType;
      m_reset_flag = src.m_reset_flag;
      m_callDir = src.m_callDir;
      m_sigType = src.m_sigType;
      m_rtpEp = src.m_rtpEp;
      audioDetail = src.audioDetail;
      m_filePath = src.m_filePath;
      m_fileSize = src.m_fileSize;
      m_pbxIp = src.m_pbxIp;
      m_pbxPort = src.m_pbxPort;
      m_ptime = src.m_ptime;
      m_maxPtime = src.m_maxPtime;
      m_adHoc = src.m_adHoc;
      m_dtmfPt = src.m_dtmfPt;
      m_mrcp_port_activate = src.m_mrcp_port_activate;
      m_mrcp_port_allocate = src.m_mrcp_port_allocate;
      m_mrcp_ip_info = src.m_mrcp_ip_info;
      m_mrcp_port = src.m_mrcp_port;
      m_mrcp_codec_name = src.m_mrcp_codec_name;
      m_mrcp_codec =src.m_mrcp_codec;
      m_mrcp_ptime = src.m_mrcp_ptime;
      m_mrcp_mg_port = src.m_mrcp_mg_port;
      m_mrcp_mg_ip = src.m_mrcp_mg_ip;
      m_mrcp_server_port = src.m_mrcp_server_port;
      m_mrcp_server_ip = src.m_mrcp_server_ip;
      m_mrcpUid = src.m_mrcpUid;
      m_mrcpCallid = src.m_mrcpCallid;
      m_mrcpDomainid = src.m_mrcpDomainid;
      m_mrcp_Devicetype= src.m_mrcp_Devicetype;
      m_mrcp_Ext = src.m_mrcp_Ext; 
      m_mrcp_isSttEnabled = src.m_mrcp_isSttEnabled;
      m_transcript_mode = src.m_transcript_mode;
      m_tts_enabled = src.m_tts_enabled;
      m_mg_Ip = src.m_mg_Ip;
      m_mg_port = src.m_mg_port;
      m_audioTtsPort = src.m_audioTtsPort;
      m_audioPort = src.m_audioPort;
      m_transExt = src.m_transExt;
      m_ccaas_block_id=src.m_ccaas_block_id;
      m_ccaas_block_type=src.m_ccaas_block_type;
      m_ccaas_channel_id=src.m_ccaas_channel_id;
      m_tts_fd = src.m_tts_fd;
      m_ccaas_voice_recog_port=src.m_ccaas_voice_recog_port;
      m_ccaas_agent_id=src.m_ccaas_agent_id;
      m_ccaas_bot_url = src.m_ccaas_bot_url;
      return *this;
   };
};

class xGateHmpGstUtil {
   private:
      xGateHmpGstUtil();
      ~xGateHmpGstUtil();
      xGateHmpGstUtil(const xGateHmpGstUtil& rhs);
      xGateHmpGstUtil& operator= (const xGateHmpGstUtil& rhs);
      static guint16 m_dtmfLowDurationLimit;
      static guint16 m_enabledecresampler;
      static guint16 m_enabledecconverter;
      static guint16 m_enableg729decresampler;
      static guint16 m_enableg729decconverter;
      static guint16 m_enableg729decrate;
   public:
      static GSocket * create_udpsocket(gint udpPort, unsigned short IpFamily = G_SOCKET_FAMILY_IPV4);
      static void generateUniqueStr(char *pout, int len);
      static guint16 getDtmfLowDurationLimit();
      static void setDtmfLowDurationLimit(unsigned int value);
      static guint16 getenabledecresampler();
      static void setenabledecresampler(unsigned int value);
      static guint16 getenabledecconverter();
      static void setenabledecconverter(unsigned int value);
      static guint16 getenableg729decresampler();
      static void setenableg729decresampler(unsigned int value);
      static guint16 getenableg729decconverter();
      static void setenableg729decconverter(unsigned int value);
      static guint16 getenableg729decrate();
      static void setenableg729decrate(unsigned int value);
      static gboolean is_regular_file(string path);
      static guint file_size(string path);
};




#endif
