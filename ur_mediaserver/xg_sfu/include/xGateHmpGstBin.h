#ifndef _XGATE_HMP_GST_BIN_H
#define _XGATE_HMP_GST_BIN_H

//gstreamer include
#include <stdlib.h>
#include <stdio.h>
#include <gst/gst.h>
#include <fcntl.h>
#include <glib.h>
#include <gio/gio.h>
#include <gst/net/gstnet.h>
#include <pthread.h>
#include <gst/sdp/sdp.h>
#include <gst/webrtc/webrtc.h>
#include <map>
#include <unordered_map>
#include <ace/Task.h>
#include <ace/Singleton.h>

//Added for thread lock mechanism
#include <mutex>
#include <condition_variable>
//local include
#include "xGateUtil.h"
#include "xGateMgMsg.h"
#include "xGateHmpGstUtil.h"
#include "xGateHmpGstAudioBin.h"
#include "xGateHmpGstRateControl.h"
#include "URInhousesttclient.h"

#define HAVE_GST_DEBUG_PRINTS TRUE
#define SET_THREAD_NAME(thr_id, thr_name) pthread_setname_np(thr_id,thr_name)

typedef enum {
  XGATE_CLIENT_CREATE_RECV_CHANNEL = 0,
  XGATE_CLIENT_CREATE_SEND_CHANNEL,
  XGATE_CLIENT_CREATE_CHANNEL,
  XGATE_CLIENT_CREATE_OFFER,
  XGATE_CLIENT_SDP_UPDATE_REQUEST,
  XGATE_CLIENT_DELETE_RECV_CHANNEL,
  XGATE_CLIENT_DELETE_SEND_CHANNEL,
  XGATE_CLIENT_DELETE_ALL_CHANNEL,
  XGATE_CLIENT_PAD_ADDED,
  XGATE_CLIENT_PAD_REMOVED,
  XGATE_CLIENT_RCVD_RTP_DATA,
  XGATE_CLIENT_SEND_RTP_DATA,
  XGATE_CLIENT_SEND_KEY_FRAME_REQUEST,
  XGATE_CLIENT_ICE_CANDIDATE_GENERATED,
  XGATE_CLIENT_PEER_CONNECTION_FAILED,
  XGATE_CLIENT_SEND_UPDATED_SDP,
  XGATE_CLIENT_UPDATE_SCREEN_SHARE_ID,
  XGATE_CLIENT_THREAD_STOP,
  XGATE_CLIENT_SEND_EVENT_SINK_PAD,
  XGATE_CLIENT_REC_EVENT_REQUEST,
  XGATE_CLIENT_REC_EVENT_RESPONSE,
  XGATE_CLIENT_TILE_CHANGE_REQUEST,
  XGATE_CLIENT_SCREENSHARE_REQUEST,
  XGATE_CLIENT_ACTIVE_SPEAKER_REQUEST,
  XGATE_CLIENT_ADD_AUDIO_MIXER_PAD,
  XGATE_CLIENT_MIX_AUDIO,
  XGATE_CLIENT_DECODE_AUDIO,
  XGATE_CLIENT_ENCODE_AUDIO,
  XGATE_CLIENT_MAX_MSG
}xGateHmpClientMsgType;

typedef enum _xGateDeviceType {
  XGATE_DEVICE_TYPE_UNKNOWN,
  XGATE_DEVICE_TYPE_MOBILE
}xGateDeviceType;

typedef enum xGateMid3CallType{
  XGATE_MID3_CALL_TYPE_NONE = 0,
  XGATE_MID3_CALL_TYPE_ACTIVE_SPEAKER,
  XGATE_MID3_CALL_TYPE_SCREENSHARE,
} xGateMid3CallType;

typedef enum xGateViewType{
  XGATE_VIEW_TYPE_GRID = 0,
  XGATE_VIEW_TYPE_ACTIVE_SPEAKER
} xGateViewType;

//Active speaker detection related defines
//2 sec schedule to detect Active speaker
#define WAIT_TIME_USEC       2000000
#define MAX_NUM_SAMPLES      5
#define SILENCE_SAMPLE_VALUE_DB (gdouble) -112.000
#define RMS_PERCENT_VARIATION 2
// Audio level message to be recieved by application in nano seconds
#define AUDIO_LEVEL_DETECT_DURATION (WAIT_TIME_USEC/MAX_NUM_SAMPLES ) * 1000

#define MIXER_WAIT_TIME_USEC 20000 // 20 ms
//#define MIXER_WAIT_TIME_USEC 19700 // 20 ms
#define MIXER_WAIT_TIME_MILLISEC 20 // 20 ms
#define SAMPLE_DURATION_20MS     20

#define DUMP_MIXER_OUTPUT_DATA   1
#define DUMP_MIXER_INPUT_DATA    2
#define DUMP_DECDODER_DATA       3
#define AUDIO_DUMP_FILE_PATH     "/root/"
//#define AUDIO_MIXER_FILE_DUMP
// latency configured in nano sec gstreamer mixer to buffer the incoming data in pads
#define GST_AUDIO_MIXER_LATENCY   20000000 //20 ms
#define GST_AUDIO_MIXER_THREADSHOLD   100000000 // 100 ms

typedef struct AudioSampleInfo {
  gdouble peak_dB;
  gdouble rms_dB;
  gdouble decay_dB;
  char    *callId_str;
} AUDIO_SAMPLE_INFO;


class xGateHmpClientThreadMsg : public ACE_Message_Block
{
public:
    xGateHmpClientMsgType m_msgType;
    Client *pClient;
    void *param1;
    void *param2;
    void *param3;
    char ucName[40];
    string str_param;
    unsigned long long ullMsgNumber;
};

/* Client class - Start
*/

class Client : public ACE_Task<ACE_MT_SYNCH> {
  public:
    Client();
    Client(ClientDetail &detail);
    ~Client();

    //public member properties
    ClientDetail m_detail;
    HmpMRCPVoiceCaptureChannel *m_pHmpMRCPVoiceCaptureChannel;
    unsigned int m_audioPayload;
    //audio video channel using webrtcbin
    HmpFrameChannel *m_FrameChannel;
   /*
     * Below map is used to retrieve the frameRecvBin pointer
     * from GstElement
     */
    MLINE_OFFER_MAP  m_mlineOffer_map;
    MLINE_OFFER_VECTOR m_mlineOffer_vector;
    MLINE_OFFER_MAP  m_mlineOffer_tile_map;
    AUDIO_SAMPLE_INFO  m_sample_info[MAX_NUM_SAMPLES];
    std::mutex m_sampleInfo_mutex;
    GMutex     m_data_process_lock;
    GMutex     m_audio_decode_process_lock;
    GMutex     m_audio_encode_process_lock;
    guint32    m_last_index;
    TWCCStats sTwccAvgStat;
    RateControl *m_RateControl;
    bool m_bitrate_is_initialized;
    guint sent_bitrate;
    string m_mid3CallId;
    xGateMid3CallType m_mid3CallType;
    xGateViewType  m_viewType;
    xGateHmpClientAsrType m_asrType;
    gboolean m_hostJoined;
    gboolean m_StopSendRecvData;
    gboolean m_isHost;
    gboolean m_isConference;
 #ifdef AUDIO_MIXER_FILE_DUMP
    FILE     *fp;
    FILE     *in_fp1;
    FILE     *in_fp2;
    FILE     *dec_fp1;
    FILE     *dec_fp2;
 #endif
    unsigned int m_clientCount;
    xGateDeviceType eDeviceType;
    gboolean twcc_Stats_flag =0;
    unsigned long long ullNextAudioMsgNum;
    unsigned long long ullNextVideoMsgNum;
    unsigned long long ullLastProcessedAudioMsgNum;
    unsigned long long ullLastProcessedVideoMsgNum;
    unsigned long long ullNextAudioDecodedMsgNum;
    unsigned long long ullLastAudioDecodedMsgNum;
    unsigned long long ullNextAudioEncodeMsgNum;
    unsigned long long ullLastAudioEncodeMsgNum;
    ACE_thread_t rcv_audio_data_thrId;
    ACE_thread_t rcv_video_data_thrId;
    ACE_thread_t rcv_data_thrId;
    ACE_thread_t send_audio_data_thrId;
    ACE_thread_t send_video_data_thrId;
    ACE_thread_t send_data_thrId;
 #ifndef USE_GST_AUDIO_MIXER_PLUGIN
    ACE_thread_t audio_mixer_thrId;
    vector <ACE_Message_Block*> vInpBuffersToMix;
    ACE_Message_Queue<ACE_MT_SYNCH, ACE_System_Time_Policy>  *mixer_msg_q;
    gboolean peek_data_avail_participants ();
 #endif
    ACE_Message_Queue<ACE_MT_SYNCH, ACE_System_Time_Policy>  *send_audio_msg_q;
    ACE_Message_Queue<ACE_MT_SYNCH, ACE_System_Time_Policy>  *send_video_msg_q;
    ACE_Message_Queue<ACE_MT_SYNCH, ACE_System_Time_Policy>  *send_data_msg_q;
    ACE_Message_Queue<ACE_MT_SYNCH, ACE_System_Time_Policy>  *rcv_data_msg_q;
    //public member functions
#ifdef RECORD_ENABLED
    GAsyncQueue *m_rcvd_audio_queue;
    GAsyncQueue *m_rcvd_video_queue;
    static void free_rcvd_audio_queue(gpointer data);
    static void free_rcvd_video_queue(gpointer data);
#endif
    void stop_data_threads (Client *client);
    gboolean add_callId(string,HmpFrameChannel*);
    void set_thread_state (gboolean);
    HmpFrameChannel* create_frame_send_channel(string call_Id, string remote_sdp);
    //bool process_client_msg(ACE_Message_Block *pAmb);
    void process_client_msg(xGateHmpClientThreadMsg *pMsg);
    void process_rcvd_stats (Client* client, TWCCStats *pTwccStat, RTPRecvReportStats *pRRStat);
    void accumulate_rtcp_stats(Client *client, RTPRecvReportStats *pRRStat);
    void compare_rtcp_params (mlineofferDetail *pOffer, guint uiMediaType, RTPRecvReportStats *pRRStat);
    void check_update_transmit_resolution (Client* client, xGateRateControlState control_state);
    static void on_twcc_stats_recv (GstElement *rtpsession, GstStructure *pTwccStats, gpointer data);
    static void on_rtp_stats_recv (GstElement *rtpsession, GstStructure *prtpStats, gpointer data);
    void get_twcc_stats(HmpFrameBin *pFrameBin, TWCCStats *pCurrentTwccStat);
    void get_rtp_stats ();
    void handle_client_delete_channel (xGateHmpClientMsgType eMsgType, char *callId_str,
		                       HmpFrameChannel *pHmpFrameChannel);
    static int client_thr_func(void *arg);
    static void client_rcv_audio_data_thr_func(Client *client);
    static void client_rcv_video_data_thr_func(Client *client);
    static void client_rcv_data_thr_func(Client *client);
    static void client_send_video_data_thr_func(Client *client);
    static void client_send_audio_data_thr_func(Client *client);
    static void client_send_data_thr_func(Client *client);
    void handle_audio_level_msg (Client *client, GstStructure *gst_struct);
    xGateRetVal spawn_audio_mixer ();
    static void mix_participants_audio(void *pVoidObj);
    void add_buffers (Client *src_client, gchar *element_name,
                             gpointer inp, gpointer out, guint in_size, guint out_size);
    static void DumpAudioData (Client *dst_client, guint32 u32Type, gchar *i8Data, guint32 u32size , Client *src_client);
    static xGateDeviceType find_device_type (string &str);
    gboolean m_data_rcv_thr_stop;
    gboolean send_tmmbr_message_set;
    ACE_Thread_Mutex m_ace_lock;
    static void create_data_thread (Client *client, xGateHmpClientMsgType eMsgType, guint eMediaType);
    URInhousesttclient *m_inhouseClient;
    gboolean CreateInhouseClient(std::string roomId,std::string connId,std::string deviceId,unsigned int isCcaas);
    gboolean restartRecognization();
    gboolean create_voice_capture_channel();
    static void flush_msg_queue (ACE_Message_Queue<ACE_MT_SYNCH, ACE_System_Time_Policy> *msg_q);
    gboolean handle_active_speaker_request(PARTICIPANT_LIST *Participant_list);
#ifdef RECORD_ENABLED
    HmpRecordSendChannel* get_client_record_channel();
    void delete_recording_channel(Client *client);
    void handle_send_stop_recording(Client *client, HmpRecordSendChannel *channel);
    gboolean handle_recording_request(xGateMediaEvent recEvent);
    gboolean handle_recording_response(xGateMediaEvent recEvent);
    void handle_send_stop_ssrc_recording(guint audio_ssrc, guint video_ssrc,Client *client);
    gboolean handle_tile_change_request(PARTICIPANT_LIST *Participant_list);
#endif
  private:
    //private member functions
    string form_channel_name(string name);
    string form_videochannel_name(string name);
    gboolean create_frame_channel(guint uiwebrtcCount);
    HmpRecordSendChannel * create_record_send_channel();
    gboolean handle_sdp_update_request(PARTICIPANT_LIST *Participant_list, string remote_sdp);
    gboolean handle_screenshare_request(PARTICIPANT_LIST *Participant_list);
    gboolean m_run;
    gboolean m_twcc_thr_run;
    xGateWebRtcBinType m_webrtcType;
};
/* Client class - End
*/

/* Dummy callid to be inserted in map. since map sorts elements after insertion, this
 * entry will be in end
 */
#define DUMMY_CALLID "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"
#define EMPTY_DUMMY_CALLID "zzzzzzzzzzzzzzzzzzzzz_empty"

//0 - will be allocated for audio_only,
//1 - will be for screenshare/active participant video.

#define INITIAL_MLINE_VECTOR_COUNT 2

//xGateHmpConference class implementation - Start 

typedef map<string, Client*> CLIENT_MAP;
typedef vector<Client*> CLIENT_CONNECTED_ORDER;
#ifdef RECORD_ENABLED
typedef map<string, HmpRecordSendChannel*> RECORDER_MAP;
#endif
class xGateHmpConference {
  public:
    //public member properties
    string m_conferenceId;
    xGateCallType m_confType;
    gboolean m_isHost;
    gboolean m_hostJoined;
    guint    m_webrtcCount;
    gboolean m_StopConference;
    CLIENT_MAP m_clientMap;
    CLIENT_CONNECTED_ORDER m_clientVector;
    string m_activeSpeakerId="";
    string m_Prev_activeSpeakerId="";

    xGateHmpConference(string conferenceId, xGateCallType confType);
    ~xGateHmpConference();
    xGateRetVal create_channel(ClientDetail &clientDetail);
    xGateRetVal create_conference_channel(Client *client);
    xGateRetVal remove_participant(Client *client);
    xGateRetVal remove_conf_participant(Client *client);
    xGateRetVal remove_send_channel(Client *client,
		                    string callId);

    //CLIENT_MAP related functions
    xGateRetVal insert_participant_entry(string callId, Client *client);
    Client * get_participant_entry(string callId);
    Client * get_ssrc_participant_entry(guint ssrc);
    xGateRetVal remove_participant_entry(string callId);
    xGateRetVal remove_participant_entries();
    xGateRetVal spawn_active_spkr_detect ();
    static void detect_active_spkr_participant (void *pVoidObj);
    xGateRetVal send_active_speaker_info(string &callId, gboolean bForceSendInfo);
    static void get_average_variation_samples(Client  *client, gdouble  *avg_rms,
                                      gdouble  *avg_peak_variation, gdouble  *avg_decay_variation);
    void fetch_last_connected_entry (void *pVoidObj);
    void find_initial_active_spkr_client ();
#ifdef RECORD_ENABLED
    HmpRecordSendChannel* get_record_send_channel(string clientId);
    xGateRetVal insert_record_send_channel(string clientId, HmpRecordSendChannel *channel);
    xGateRetVal remove_record_send_channel(string clientId);
    xGateRetVal remove_record_send_channels();

    GAsyncQueue *m_recorder_audio_queue;
    GAsyncQueue *m_recorder_video_queue;
    static void free_recorder_audio_queue(gpointer data);
    static void free_recorder_video_queue(gpointer data);
    RECORDER_MAP m_recorderMap;
#endif
};

#endif // _XGATE_HMP_GST_BIN_H
