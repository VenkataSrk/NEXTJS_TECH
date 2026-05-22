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

#define CHANNEL_NAME_SIZE 300

//local includes
#include "xGateUtil.h"

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
  EN_MEDIA_EVENT_NULL = 0, // Inital state no actin to take in MG
  EN_MEDIA_EVENT_PLAY = 1,      // state at PLAY create RBT pipeline in Mg and play RBT
  EN_MEDIA_EVENT_STOP = 2,      // State at STOP if RBT pipeline is running stop the play
  EN_MEDIA_EVENT_EOS = 3,
  EN_MEDIA_EVENT_REPEAT = 4,
  EN_MEDIA_EVENT_HOLD = 5,
  EN_MEDIA_EVENT_RESUME = 6,
  EN_MEDIA_EVENT_GETDIGIT = 7,   // Request for start to collect the DTMF Digit
  EN_MEDIA_EVENT_SENDDIGIT = 8,  // Request for send the Collected DTMF Digits.
  EN_MEDIA_EVENT_RECVDIGIT = 9,    // Posting the Collected DTMF Digits.
  EN_MEDIA_EVENT_MUTE = 10,
  EN_MEDIA_EVENT_UNMUTE = 11,
  EN_MEDIA_EVENT_DEAF = 12,
  EN_MEDIA_EVENT_LOCK = 13,
  EN_MEDIA_EVENT_UNLOCK = 14,
  EN_MEDIA_EVENT_EXTEND = 15,
  EN_MEDIA_EVENT_TRANSFER = 16,
  EN_MEDIA_EVENT_CONFALL = 17,
  EN_MEDIA_EVENT_CONF_END = 18,
  EN_MEDIA_EVENT_RECORD_START = 19, //record both audio, video and entire call
  EN_MEDIA_EVENT_RECORD_STOP = 20,
  EN_MEDIA_EVENT_RECORD_HOLD = 21,
  EN_MEDIA_EVENT_RECORD_RESUME =22,
  EN_MEDIA_EVENT_RECORD_SUCCESS = 23,
  EN_MEDIA_EVENT_RECORD_FAILED = 24,
  EN_MEDIA_EVENT_RECORD_INFO = 25,
}xGateMediaEvent;

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
  EN_PIPELINE_RECORD_MP3,
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
  EN_BIN_LOCAL_AUDIO_RECV,
  EN_BIN_RECORD_MP3, //mainly created for recording functionality
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

typedef enum
{
  EN_CANDIDATE_TYPE_UNKNOWN=0,
  EN_CANDIDATE_TYPE_HOST=1,
  EN_CANDIDATE_TYPE_PRFLX=2, //peer reflexive candidate
  EN_CANDIDATE_TYPE_SRFLX=3, //server reflexive candidate
  EN_CANDIDATE_TYPE_RELAY=4, //relay candidate
}xGateCandidateType;

struct CandidateDetail
{
  CandidateDetail() : m_piority(1), m_transportType(EN_XGATE_TRANSPORT_UDP),
  m_raddr(""), m_rport(0), m_candidateType(EN_CANDIDATE_TYPE_UNKNOWN)
  {};
  ~CandidateDetail(){};
  unsigned int m_piority;
  xGateTransportType m_transportType;
  string m_raddr; //remote ip address
  unsigned int m_rport; //remote port
  xGateCandidateType m_candidateType;
};

typedef vector<CandidateDetail> CANDIDATE_LIST;

class MediaSecureDetail
{
  public:
    MediaSecureDetail();
    ~MediaSecureDetail();

    //Assignment operator
    MediaSecureDetail& operator=(MediaSecureDetail &src);
    CandidateDetail m_candidateDetail;
    string m_pemFile;
    string m_lIceUfrag; //locally created ice ufrag
    string m_lIcePwd; //locally created ice password
    string m_rIceUfrag; //remote ice ufrag
    string m_rIcePwd; //remote ice password
    string m_lFingerPrint; //locally created finger print
    string m_rFingerPrint; //remote finger print

    string m_lSsrc, m_rSsrc; //local and remote ssrc
    string m_lCname, m_rCname; //local and remote cname
    string m_lMsLabel, m_rMsLabel; //local and remote mslable
    string m_lLabel, m_rLabel;

    xGateRetVal add_candidate();
    xGateRetVal remove_candidate();
    xGateRetVal clear_candidate_list();
};
   /*  MediaSecureDetail class - End
   */
class AudioDetail {
  public:
    AudioDetail();
    ~AudioDetail();
    AudioDetail& operator=(AudioDetail &src);

    unsigned int m_mediaType;
    string m_fmtp;
    unsigned int m_codec;
    string m_codecName;
    unsigned int m_ptime;
    string m_playFile;
    string m_clientIp;
    unsigned int m_clientPort;
    string m_serverIp;
    unsigned int m_serverPort;
    string m_relayIp;
    unsigned int m_relayPort;
    string m_rflxIp;
    unsigned int m_rflxPort;
    MediaSecureDetail m_secureDetail;
    string m_dialIp;
    unsigned int m_dialPort;
    string m_respIp;
    unsigned int m_respPort;
};

class VideoDetail {
  public:
    VideoDetail();
    ~VideoDetail();
    VideoDetail& operator=(VideoDetail &src);

    unsigned int m_mediaType;
    string m_fmtp;
    unsigned int m_codec;
    string m_codecName;
    unsigned int m_frameRate;
    unsigned int m_imageattr_x;
    unsigned int m_imageattr_y;
    string m_playFile;
    string m_clientIp;
    unsigned int m_clientPort;
    string m_serverIp;
    unsigned int m_serverPort;
    string m_relayIp;
    unsigned int m_relayPort;
    string m_rflxIp;
    unsigned int m_rflxPort;
    MediaSecureDetail m_secureDetail;
    string m_dialIp;
    unsigned int m_dialPort;
    string m_respIp;
    unsigned int m_respPort;
};

struct ClientDetail {
  string m_mgresourceId;
  xGateCallType m_callType;
  string m_callId;
  unsigned int m_gateway_id;
  int m_legId;
  int m_context_id;
  HmpMediaMode m_mediaModeIn;
  HmpMediaMode m_mediaModeOut;
  string m_dtmfBuffer;
  xGateMediaEvent m_mediaEvent;
  xGateJoinerType m_joinerType;
  HmpCallDir m_callDir;
  unsigned int m_sigType;
  string m_rtpEp;
  AudioDetail audioDetail;
  VideoDetail videoDetail;
  string m_filePath;
  long int m_fileSize;
  string m_pbxIp;
  unsigned int m_pbxPort;

  ClientDetail() : 
    m_mgresourceId(""), m_callType(EN_XGATE_CALL_TYPE_UNKNOWN), m_callId(""), m_gateway_id(0), m_legId(-1), 
    m_context_id(0), m_mediaModeIn(EN_MEDIA_UNKNOWN), m_mediaModeOut(EN_MEDIA_UNKNOWN), m_dtmfBuffer(""),
    m_mediaEvent(EN_MEDIA_EVENT_NULL), m_joinerType(EN_XGATE_JOINER_TYPE_UNKNOWN), m_callDir(EN_CALL_DIR_UNKNOWN),
    m_sigType(3), m_rtpEp(""), m_filePath(""), m_fileSize(0), m_pbxIp(""), m_pbxPort(0)
  {};

  ClientDetail& operator=(ClientDetail &src)
  {
    m_mgresourceId = src.m_mgresourceId;
    m_callType = src.m_callType;
    m_callId = src.m_callId;
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
    return *this;
  };
};

class xGateHmpGstUtil {
  private:
    xGateHmpGstUtil();
    ~xGateHmpGstUtil();
    xGateHmpGstUtil(const xGateHmpGstUtil& rhs);
    xGateHmpGstUtil& operator= (const xGateHmpGstUtil& rhs);
  public:
    static GSocket * create_udpsocket(gint udpPort);
    static void generateUniqueStr(char *pout, int len);
    static void makeDir(const char *dir);
    static unsigned int find_wav_duration(string path);
    static unsigned int file_size(string path);

};

#endif
