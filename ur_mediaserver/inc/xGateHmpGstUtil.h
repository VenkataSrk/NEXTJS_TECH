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

#include <string>
//#define GST_DEBUG TRUE
#define LOOPBACK_IP "127.0.0.1"
#define CONNECTSTR "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define CONNECTSTR_LEN strlen(CONNECTSTR)
using namespace std;
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
 EN_BIN_PRESENT_UNKNOWN=0,
 EN_BIN_PRESENT_RECV_END,
 EN_BIN_PRESENT_SEND_END

}BinPresentState;

typedef enum
{
  EN_PIPELINE_UNKNOWN,
  EN_PIPELINE_VOICE_RECV,
  EN_PIPELINE_VOICE_SEND,
  EN_PIPELINE_VOICE_DEC,
  EN_PIPELINE_VOICE_ENC,
  EN_PIPELINE_FRAME_RECV,
  EN_PIPELINE_FRAME_SEND,
  EN_PIPELINE_FRAME_DEC,
  EN_PIPELINE_FRAME_ENC,
  EN_PIPELINE_PLAY_AUDIO,
  EN_PIPELINE_PLAY_FRAME,
  EN_PIPELINE_RECORD_AUDIO,
  EN_PIPELINE_RECORD_FRAME,
  EN_PIPELINE_CONFERENCE_AUDIO,
  EN_PIPELINE_VOICE_CAPTURE,
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
  EN_BIN_PLAY_AUDIO,
  EN_BIN_PLAY_FRAME,
  EN_BIN_RECORD_AUDIO,
  EN_BIN_RECORD_VEDIO,
  EN_BIN_CONFERENCE_AUDIO,
  EN_BIN_LOCAL_AUDIO_SEND,
  EN_BIN_LOCAL_AUDIO_RECV, //mainly created for recording functionality
} HmpBinType;
//Enum declaration - end

typedef enum
{
  EN_BIN_VID_UNKNOWN,
  EN_BIN_VID_VP8_SEND,
  EN_BIN_VID_VP8_RECV,
  EN_BIN_VID_VP9_SEND,
  EN_BIN_VID_VP9_RECV,
  EN_BIN_VID_H264_SEND,
  EN_BIN_VID_H264_RECV,
  EN_BIN_VID_H265_SEND,
  EN_BIN_VID_H265_RECV,
  EN_BIN_VID_DTLS_RECV,
  EN_BIN_VID_DTLS_SEND,
  EN_BIN_VID_PLAY_FRAME,
} HmpVideoBinType;

struct ClientDetail {
  string m_mgresourceId;
  xGateCallType m_callType;
  string m_callId;
  string m_conn_id;
  string m_device_id;
  unsigned int m_gateway_id;
  int m_legId;
  int m_context_id;
  HmpMediaMode m_mediaModeIn;
  HmpMediaMode m_mediaModeOut;
  string m_dtmfBuffer;
  xGateMediaEvent m_mediaEvent;
  string m_screenshare_id;
  xGateJoinerType m_joinerType;
  HmpCallDir m_callDir;
  unsigned int m_sigType;
  string m_rtpEp;
  string m_sdpStr;
  AudioDetail audioDetail;
  VideoDetail videoDetail;
  string m_filePath;
  long int m_fileSize;
  string m_pbxIp;
  unsigned int m_pbxPort;
  string m_record_file;
  string m_browser_type;
  string m_record_url;
  string m_meeting_name;
  string m_sip_id;
  xGateHmpClientAsrType m_asrType;
  USERINFO meet_userinfo;
  unsigned int retrieved_videossrc;
  unsigned int retrieved_audiossrc;
  unsigned int is_ccaas;

  ClientDetail() : 
    m_mgresourceId(""), m_callType(EN_XGATE_CALL_TYPE_UNKNOWN), m_callId(""), m_conn_id(""),m_device_id(""),m_gateway_id(0), m_legId(-1), 
    m_context_id(0), m_mediaModeIn(EN_MEDIA_UNKNOWN), m_mediaModeOut(EN_MEDIA_UNKNOWN),m_screenshare_id(""), m_dtmfBuffer(""),
    m_mediaEvent(EN_MEDIA_EVENT_NULL), m_joinerType(EN_JOINER_TYPE_UNKNOWN), m_callDir(EN_CALL_DIR_UNKNOWN),
    m_sigType(3), m_rtpEp(""), m_filePath(""), m_fileSize(0), m_pbxIp(""), m_pbxPort(0),m_sdpStr(""),m_record_file(""),m_browser_type(""),m_record_url(""),m_sip_id(""),m_meeting_name(""),
    m_asrType(EN_XGATE_ASR_TYPE_UNKNOWN),retrieved_videossrc(0),retrieved_audiossrc(0),is_ccaas(0)
  {};

  ClientDetail& operator=(ClientDetail &src)
  {
    m_mgresourceId = src.m_mgresourceId;
    m_callType = src.m_callType;
    m_callId = src.m_callId;
    m_conn_id = src.m_conn_id;
    m_device_id = src.m_device_id;
    m_gateway_id = src.m_gateway_id;
    m_legId = src.m_legId;
    m_context_id = src.m_context_id;
    m_mediaModeIn = src.m_mediaModeIn;
    m_mediaModeOut = src.m_mediaModeOut;
    m_dtmfBuffer = src.m_dtmfBuffer;
    m_joinerType = src.m_joinerType;
    m_callDir = src.m_callDir;
    m_sigType = src.m_sigType;
    m_rtpEp = src.m_rtpEp;
    audioDetail = src.audioDetail;
    videoDetail = src.videoDetail;    
    m_filePath = src.m_filePath;
    m_fileSize = src.m_fileSize;
    m_pbxIp = src.m_pbxIp;
    m_pbxPort = src.m_pbxPort;
    m_screenshare_id = src.m_screenshare_id;
    m_sdpStr = src.m_sdpStr;
    m_mediaEvent = src.m_mediaEvent;
    m_record_file = src.m_record_file;
    m_browser_type = src.m_browser_type;
    m_record_url = src.m_record_url;
    m_meeting_name = src.m_meeting_name;
    m_sip_id = src.m_sip_id;
    m_asrType = src.m_asrType;
    retrieved_videossrc = src.retrieved_videossrc;
    retrieved_audiossrc = src.retrieved_audiossrc;
    is_ccaas = src.is_ccaas;
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
  public:
    static GSocket * create_udpsocket(gint udpPort);
    static void generateUniqueStr(char *pout, int len);
    static guint16 getDtmfLowDurationLimit();
    static void setDtmfLowDurationLimit(unsigned int value);
};

typedef enum
{
 EN_WEBRTC_TYPE_UNKNOWN=0,
 EN_WEBRTC_TYPE_SENDRECV=1,
 EN_WEBRTC_TYPE_SENDONLY=2,
 EN_WEBRTC_TYPE_RECVONLY=3,

}xGateWebRtcBinType;

typedef enum
{
   EN_SDP_TYPE_UNKNOWN = 0,
   EN_SDP_TYPE_AUDIO = 1,
   EN_SDP_TYPE_VIDEO = 2,
   EN_SDP_TYPE_AUDIO_VIDEO = 3,
}xGateSDPType;

#endif
