#ifndef _XGATE_HMP_GST_BIN_H
#define _XGATE_HMP_GST_BIN_H

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

#define GST_DEBUG FALSE

#define HOST_RTP_PORT_START   10000
#define HOST_RTP_PORT_SPAN    25000
int rtp_port=HOST_RTP_PORT_START;
#define HOSTGST_RTP_GET_PORT() ((rtp_port) > (HOST_RTP_PORT_SPAN)?(rtp_port=HOST_RTP_PORT_START):(rtp_port=rtp_port+2))

#define HOSTGST_TRM_PORT_START  25000
#define HOSTGST_TRM_PORT_END    40000
int gst_trm_port=HOSTGST_TRM_PORT_START;
#define HOSTGST_TRM_GET_PORT() ((gst_trm_port >= HOSTGST_TRM_PORT_END)?(gst_trm_port=HOSTGST_TRM_PORT_START):(gst_trm_port+=2))


#define UDP_CAPS_L16 "audio/x-raw, format=(string)S16BE, layout=(string)interleaved, rate=(int)8000, channels=(int)1"
#define UDP_CAPS_RTPL16 "application/x-rtp, media=(string)audio, clock-rate=(int)8000"
#define UDP_CAPS_G711A "application/x-rtp, media=(string)audio, clock-rate=(int)8000, encoding-name=(string)PCMA, payload=(int)8"
#define UDP_CAPS_G711U "application/x-rtp, media=(string)audio, clock-rate=(int)8000, encoding-name=(string)PCMU, payload=(int)8"
#define UDP_CAPS_G729 "application/x-rtp, media=(string)audio, clock-rate=(int)8000, encoding-name=(string)G729, payload=(int)18"
#define UDP_CAPS_OPUS "application/x-rtp,media=(string)audio,clock-rate=(int)48000,encoding-name=(string)X-GST-OPUS-DRAFT-SPITTKA-00"
#define UDP_CAPS_DTLS "application/x-rtp;application/x-dtls;application/x-stun;application/x-srtp"

#define RTP_CAPS "application/x-rtp, media=(string)audio, clock-rate=(int)8000"
#define CAPS_DEC_OPUS "application/x-rtp,media=(string)audio, payload=(int)111, clock-rate=(int)48000,encoding-name=(string)X-GST-OPUS-DRAFT-SPITTKA-00"

#define CAPS_ENC_OPUS "application/x-rtp,media=(string)audio,clock-rate=(int)48000,encoding-name=(string)X-GST-OPUS-DRAFT-SPITTKA-00"

#define CONNECTSTR "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define CONNECTSTR_LEN strlen(CONNECTSTR)

typedef enum
{
  EN_UNKNOWN_CODEC = -1,
  EN_RTP_CODEC = 1, 
  EN_PCMU_CODEC = 0,
  EN_PCMA_CODEC = 8,
  EN_G722_CODEC = 9,
  EN_CNF_CODEC = 13,
  EN_G729_CODEC = 18,
  EN_OPUS_CODEC = 111,
} HmpGstCodecType;

typedef enum
{
  EN_PIPELINE_UNKNOWN,
  EN_PIPELINE_SINGLE_CALL,
  EN_PIPELINE_CONF_CALL,
  EN_PIPELINE_ADD_CALL,
  EN_PIPELINE_PLAY_BACK,
}HmpGstPipelineType;

typedef enum
{
  EN_BIN_UNKNOWN,
  EN_BIN_RTP,
  EN_BIN_G711,
  EN_BIN_G729,
  EN_BIN_OPUS,
  EN_BIN_PLAY_FILE,
}HmpGstBinType;

typedef enum
{
  EN_MEDIA_UNKNOWN = 0,
  EN_MEDIA_RTP = 1,
  EN_MEDIA_SRTP = 2,
  EN_MEDIA_DTLS = 3,
}HmpGstMediaMode;


typedef enum
{
  EN_CALL_DIR_UNKNOWN = 0,
  EN_CALL_DIR_IN = 1,
  EN_CALL_DIR_OUT = 2,
} HmpGstCallDir;

typedef struct _GstSrtpCaps {
    gboolean key_is_set;
    guint rtpssrc;
    GstBuffer *srtp_key;
    gchar srtp_cipher[100];
    gchar srtp_auth[100];
    gchar srtcp_cipher[100];
    gchar srtcp_auth[100];
}GstSrtpCaps;

typedef struct securityParams
{
  guint sockfd;
  gchar targetIp[50];
  guint targetPort;
  gchar cryptoLine[256];
  gchar iceufrag[100];
  gchar icepwd[100];
  gchar pemFile[100];
  gchar connectId[20];
} SecurityParams;

//class HmpGstBinFactory;
//class xGateHmpGstBin;
class HmpUdpTermBin;
class HmpGstVoiceChannel;
class HmpGstPlayChannel;

struct ClientDetail {
  string m_callId;
  unsigned int m_legid;
  string m_ip;
  unsigned int m_port;
  string m_serverIp;
  unsigned int m_serverPort;
  string m_relayIpaddr;
  unsigned int m_relayPort;
  string m_reflexIpaddr;
  unsigned int m_reflexPort;
  HmpGstCodecType m_codec;
  string m_remIceUfrag;
  string m_locIceUfrag;
  string m_icePwd;
  string m_fingerPrint;
  string m_pemFile;
  string m_ssrc;
  string m_cname;
  string m_mslabel;
  string m_label;
  unsigned int m_pTime;
  xGatePbState m_pbState;
  string m_dtmfBuffer;
  HmpGstMediaMode m_mediaMode;
  HmpGstMediaMode m_outProto;
  HmpGstCallDir m_callDir;
  unsigned int m_sigType;

  ClientDetail(): m_callId(""), m_ip(""), m_port(0),
    m_serverIp(""), m_serverPort(0), m_codec(EN_UNKNOWN_CODEC),
    m_callDir(EN_CALL_DIR_UNKNOWN), m_sigType(0)
  {
  };
  ClientDetail& operator=(ClientDetail &src)
  {
    m_callId = src.m_callId;
    m_legid = src.m_legid;
    m_ip = src.m_ip;
    m_port = src.m_port;
    m_serverIp = src.m_serverIp;
    m_serverPort = src.m_serverPort;
    m_relayIpaddr = src.m_relayIpaddr;
    m_relayPort = src.m_relayPort;
    m_reflexIpaddr = src.m_reflexIpaddr;
    m_reflexPort = src.m_reflexPort;
    m_codec = src.m_codec;
    m_remIceUfrag = src.m_remIceUfrag;
    m_locIceUfrag = src.m_locIceUfrag;
    m_icePwd = src.m_icePwd;
    m_fingerPrint = src.m_fingerPrint;
    m_pTime = src. m_pTime;
    m_pbState = src.m_pbState;
    m_dtmfBuffer = src.m_dtmfBuffer;
    m_mediaMode = src.m_mediaMode;
    m_outProto = src.m_outProto;
    m_callDir = src.m_callDir;
    m_sigType = src.m_sigType;
    return *this;
  };
};

struct Client {
  ClientDetail m_detail;
  unsigned int m_terPort;
  HmpGstVoiceChannel *m_voiceChannel;
  HmpGstPlayChannel *m_playChannel;

  Client(): m_terPort(0), m_voiceChannel(NULL), m_playChannel(NULL)
  {
  }

  Client(ClientDetail &detail) : m_detail(detail), 
    m_voiceChannel(NULL), m_playChannel(NULL)
  {
  };

  Client(HmpGstVoiceChannel *m_voiceChannel, ClientDetail &detail):
    m_detail(detail), m_voiceChannel(m_voiceChannel)
  {
  };

};

/*
 * HmpGstBin Base class for different codec bin
*/
class xGateHmpGstBin {
  public:
    xGateHmpGstBin(string binName, HmpGstBinType binType);
    ~xGateHmpGstBin();

    GstBin *m_sendBin;
    string m_sendBinName;
    GstBin *m_recvBin;
    string m_recvBinName;

    GstElement *m_sendUdp;
    GstElement *m_recvUdp;
    HmpGstMediaMode m_mediaMode;
    HmpGstBinType m_binType;
    GstSrtpCaps *gstSrtpCaps; 
    guint srtp_auth;
    GstCaps *caps, *rcaps;
    guint latency;
    SecurityParams m_params;
    unsigned int m_pTime;
    unsigned int m_udpPort;
    GSocket *m_udpSocket;
    string m_clientIp;
    unsigned int m_clientPort;

    GstElement *m_payEnc;
    GstElement *m_rtpPay;
    GstElement *m_rtpDepay;
    GstElement *m_payDec;
    GstElement *m_audioConvert;
    GstElement *m_audioResample;
    GstElement *m_capsFilter;
    GstElement *m_encFilter;
    GstElement *m_decFilter;
    GstElement *m_srtpEnc;
    GstElement *m_srtpDec;
    GstElement *m_dtlssrtpEnc;
    GstElement *m_dtlssrtpDec;
    GstElement *m_rtpJitterBuffer;
    GstElement *m_dtmfDetect;
    GstElement *m_rtpBin;
    GstElement *m_cnfUdpSink;

    GstPad *m_ghostRtpSrcPad;
    GstPad *m_ghostRtpSinkPad;
    GstPad *m_ghostInvertSinkPad;
    GstPad *m_ghostInvertSrcPad;
    GstPad *m_ghostTeeSrcPad;
    GstPad *m_ghostMixedAudioSinkPad;
    GstPad *m_ghostAuConSinkPad;
    GstPad *m_ghostDecSrcPad;
      
    //defualt implementation. subclass will do implementation 
    virtual GstBin *sendBin_create();
    virtual GstBin *recvBin_create();
    
    gboolean sendPlayBin_create();
    gboolean recvPlayBin_create();

    void generateConnectId(char *pout, int len);
    void set_client_addr(string clientIp, unsigned int clientPort);
    void set_client_details(ClientDetail &cliDetail);
    GSocket* create_udp_socket(unsigned int udpPort);
    void set_udp_socket(GSocket *udpSocket);
    gboolean create_udp_elements();

    gboolean recvBin_remove();
    gboolean sendBin_remove();

    gboolean sendBin_update();
    gboolean recvBin_update();

    GstBin * get_sendBin() {
      return m_sendBin;
    }
    void set_sendBin(GstBin *bin) {
      m_sendBin = bin;
    }
   
    GstBin * get_recvBin() {
      return m_recvBin;
    }

    void set_recvBin(GstBin *bin) {
      m_recvBin = bin;
    }

    HmpGstBinType get_bin_type() {
      return m_binType;
    }

    void set_bin_type(HmpGstBinType binType) {
      m_binType = binType;
    }

    //rtpbin element callbacks - start
    static void on_pad_added(GstElement *rtpBin, GstPad *pad, gpointer data);
    static void on_pad_removed(GstElement *rtpBin, GstPad *pad, gpointer data);
    static void on_request_pt_map(GstElement *rtpBin, guint session, guint pt, gpointer data);
    static void on_new_ssrc(GstElement *rtpBin, guint session, guint ssrc, gpointer data);
    static void on_bye_ssrc(GstElement *rtpBin, guint session, guint ssrc, gpointer data);
    static void on_ssrc_collision(GstElement *rtpBin, guint session, guint ssrc, gpointer data);
    static void on_ssrc_validated(GstElement *rtpBin, guint session, guint ssrc, gpointer data);
    //rtpbin element callbacks - end
};

/*
 * HmpG711GstBin class for encoding G711 codec
 * It will endcode G711 codec and send it to client
*/
class HmpG711Bin : public xGateHmpGstBin
{
  public:
    HmpG711Bin(string binName);
    ~HmpG711Bin();
    virtual GstBin *sendBin_create();
    virtual GstBin *recvBin_create();

  private:
    gboolean create_rtpbin();
    int sockfd;
    gchar  targetIp[50];
    int    targetPort;
};

/*
 * HmpG729GstBin class for encoding G729 codec
 * It will endcode G729 codec and send it to client
 */
class HmpG729Bin : public xGateHmpGstBin
{
  public:
    HmpG729Bin(string binName);
    ~HmpG729Bin();
    virtual GstBin * sendBin_create();
    virtual GstBin * recvBin_create();

  private:
    gboolean create_rtpbin();
    int sockfd;
    gchar  targetIp[50];
    int    targetPort;
};

/*
 *  HmpOPUSGstBin class for encoding G729 codec
 *  It will endcode G729 codec and send it to client
 */
class HmpOpusBin : public xGateHmpGstBin
{
  public:
    HmpOpusBin(string binName);
    ~HmpOpusBin();
    virtual GstBin * sendBin_create();
    virtual GstBin * recvBin_create();

  private:

    gboolean create_rtpbin();
    int sockfd;
    gchar  targetIp[50];
    int    targetPort;
};



class HmpUdpTermBin
{
  public:
    HmpUdpTermBin(string binName);
    ~HmpUdpTermBin();
    GSocket* create_udp_socket(unsigned int termPort);
    gboolean create_termBin();
    gboolean create_recvBin();
    gboolean create_sendBin();

    GstBin * get_termSendBin() {
      return m_termSendBin;
    }
    void set_termSendBin(GstBin *termSendBin) {
      m_termSendBin = termSendBin;
    }
    GstBin * get_termRecvBin() {
      return m_termRecvBin;
    }
    void set_termRecvBin(GstBin *termRecvBin) {
      m_termRecvBin = termRecvBin;
    }

    GstElement *m_udpSend; 
    GstElement *m_udpRecv;
    GstElement *m_rtpJitterBuffer;
    GstElement *m_rtpL16Pay;  
    GstElement *m_rtpL16Depay; 
    GstElement *m_rAudioConvert;
    GstElement *m_sAudioConvert; 
    GstElement *m_sCapsFilter; 
    HmpGstBinType m_binType;
    gint m_termPort;

  private:
    GstBin *m_termSendBin;
    GstBin *m_termRecvBin;
    string m_termRecvBinName;
    string m_termSendBinName;
    gint m_rtpTout;
    GSocket *m_termSocket;
};

/*
 *  HmpPlayFileBin class for playing file
 */
class HmpPlayFileBin
{
  public:
    HmpPlayFileBin(string binName, HmpGstBinType binType = EN_BIN_PLAY_FILE);
    ~HmpPlayFileBin();

    gboolean create_bin();

    GstBin * get_fileBin() {
      return m_fileBin;
    }
    void set_bin(GstBin *fileBin) {
      m_fileBin = fileBin;
    }
    GstBin * get_fakeBin() {
      return m_fakeBin;
    }
    void set_fakeBin(GstBin *fakeBin) {
      m_fakeBin = fakeBin;
    }

    HmpGstBinType get_bin_type() {
      return m_binType;
    }
    void set_bin_type(HmpGstBinType binType) {
      m_binType = binType;
    }

    GstElement *m_fileSrc;
    GstElement *m_wavParse;
    GstElement *m_fakeSink;
  private:
    unsigned short m_poolIndex; 
    GstBin *m_fileBin;
    GstBin *m_fakeBin;
    string m_fileBinName;
    string m_fakeBinName;
    HmpGstBinType m_binType;

    GstPad *m_ghostSrcPad;
    GstPad *m_ghostSinkPad;
};

/*
 * HmpGstBinFactory class for creating GstBin for various codec
*/
class HmpGstBinFactory {
  public:
    virtual HmpPlayFileBin *bin_playfile_create(string& binName, HmpGstBinType binType = EN_BIN_PLAY_FILE);
    virtual xGateHmpGstBin *bin_create(string& binName, HmpGstBinType binType, ClientDetail &cliDetail);

};

typedef vector<Client*> CLIENT_LIST;

class xGateHmpGstPipeline {
  public:
    xGateHmpGstPipeline(string pipelineName, HmpGstPipelineType m_pbSendPipelineType);
    ~xGateHmpGstPipeline();

    string m_sendVoicePipelineNameA;
    string m_recvVoicePipelineNameA;
    xGateHmpGstBin *m_voHmpBinA;
    HmpUdpTermBin *m_voTrmBinA;
//    GSocket *m_udpSocketA;
    gint m_voTrmPortA;

    string m_sendVoicePipelineNameB;
    string m_recvVoicePipelineNameB;
    xGateHmpGstBin *m_voHmpBinB;
    HmpUdpTermBin *m_voTrmBinB;
//    GSocket *m_udpSocketB;
    gint m_voTrmPortB;

    GstClock *m_pipelineClock;
    CLIENT_LIST m_clientList;
    unsigned short m_clientCount;

    gboolean support_comfort_noise(Client *sClient, Client *dClient);
    gboolean pipeline_connect_clients(Client *sClient, Client *dClient);
    gboolean disconnect_simple_client(Client* sclient, Client *dClient);

    static gboolean  handle_event_dispatcher(GstBus* bus, GstMessage* msg, gpointer data);

    gboolean update_client_detail(Client *client, ClientDetail &newDetail)
    {
      newDetail.m_serverIp = client->m_detail.m_serverIp;
      newDetail.m_serverPort = client->m_detail.m_serverPort;
      client->m_detail = newDetail;
    }

    Client * insert_client_entry(ClientDetail &detail)
    {
      Client *client = NULL;
      client = new Client(detail); 
      if(client) {
        m_clientList.push_back(client);
        ++m_clientCount;
      }
      return client;
    };

    gboolean remove_client_entry(Client *client)
    {
      CLIENT_LIST::iterator it;
      for(it = m_clientList.begin(); it != m_clientList.end(); ++it) {
        if((*it) == client) {
          m_clientList.erase(it);
          delete client;
          --m_clientCount;
          return TRUE;
        }
      }
      return FALSE;
    };

    gboolean remove_client_entries()
    {
      CLIENT_LIST::iterator it = m_clientList.begin();
      for(it; it != m_clientList.end(); ++it) {
        Client *client = (*it);
        m_clientList.erase(it);
        --m_clientCount;
        delete client;
      }
      return TRUE;
    };
};

/*
 * HmpGstVoiceChannel class for voice transfer
 */

class HmpGstVoiceChannel {

  public : 
    HmpGstVoiceChannel(string pipelineName);
    ~HmpGstVoiceChannel();
    gboolean create_voice_channel();
    gboolean activate_voice_channel(Client *sClient);
    gboolean deactivate_voice_channel(Client *sClient);
    void pipeline_stop_media(Client *sClient);
    void pipeline_start_media(Client *sClient);

    guint *m_sendVoiceBusId;
    guint *m_recvVoiceBusId;
    string m_sendVoicePipelineName;
    string m_recvVoicePipelineName;
    GstBus *m_sendVoiceBus;
    GstBus *m_recvVoiceBus;

    HmpGstBinFactory *m_binFactory;
    xGateHmpGstBin *m_voiceBin;
    HmpUdpTermBin *m_termBin;
    HmpUdpTermBin *m_plTrmBin;
    GstElement *m_recvVoicBin;
    GstElement *m_sendTermBin;
    GstElement *m_sendVoicBin;
    GstElement *m_recvTermBin;
    gint m_plTermPort;

    GstElement *m_sendVoicePipeline;
    GstElement *m_recvVoicePipeline;

    char m_dtmfBuffer[10];
    void get_dtmf_digit(Client *client);
    void send_dtmf_digit(Client *client);
    static gboolean  handle_event_dispatcher(GstBus* bus, GstMessage* msg, gpointer data);
    void post_playback_eos(Client *client);
};

/*
 *  HmpGstPlayBackPipeline class for playing file
 */
class HmpGstPlayChannel {
  public:
    HmpGstPlayChannel(string pipelineName, HmpGstPipelineType m_pbSendPipelineType = EN_PIPELINE_PLAY_BACK);
    ~HmpGstPlayChannel();
    gboolean create();
    HmpGstPipelineType m_pbSendPipelineType;
    string m_pbSendPipelineName;
    string m_pbRecvPipelineName;
    GstClock *m_pbSendPipelineClock;
    GstElement *m_pbSendPipeline;
    GstElement *m_pbRecvPipeline;
    GstBus *m_pbSendBus;
    GstBus *m_pbRecvBus;
    guint *m_pbSendBusId;
    guint *m_pbRecvBusId;
    
    xGateHmpGstBin *m_pbHmpBin;
    HmpUdpTermBin *m_pbVoTrmBin;
    HmpUdpTermBin *m_pbTermBin;
    GstElement *m_pbSendBin;
    GstElement *m_pbRecvBin;
    GstElement *m_pbFakeBin;
    GstElement *m_pbFileBin;
    gint m_pbTermPort;

    HmpPlayFileBin *m_hmpPlayFileBin;
    gboolean m_autoRepeat;

    void activate_fileplay_channel(Client *sClient, string playWavfile, gboolean autoRepeat = FALSE);
    void deactivate_fileplay_channel(Client *sClient);
    gboolean setting_playfile_channel(Client *sClient);
    static gboolean  handle_event_dispatcher(GstBus* bus, GstMessage* msg, gpointer data);
    void post_playback_eos(Client *client);
};

class HmpGstPipelineFactory {
  public:
    virtual xGateHmpGstPipeline *pipeline_create(string& pipelineName, HmpGstPipelineType pipelineType = EN_PIPELINE_SINGLE_CALL);
};

#endif
