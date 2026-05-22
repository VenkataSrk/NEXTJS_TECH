#ifndef _SFU_CONTROLLER_
#define _SFU_CONTROLLER_

/* 1. This thread object will manage gstreamer events, pipelines, bin and elements
*  2. full name of SFUController
*  3. It will manage gstreamer sources by using xGateHmpGstBin class
*/
#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>

#include "xGateUtil.h"
#include "xGateHmpGstBin.h"
#include "xGateHmpGstVideoBin.h"
#include "SFUClientMediaHandler.h"
#include "stringencode.h"
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include "xGateHmpThreadPool.h"

#define ROOT_TMP_DIR "/tmp/dtlssrtp"
#define ROOT_TMP_DIR_VID "/tmp/dtlssrtp/video"
#define TMP_DIR_TEMPLATE ROOT_TMP_DIR "/XXXXXX"
#define TMP_DIR_TEMPLATE_VID ROOT_TMP_DIR_VID "/XXXXXX"
#define FILE_PERMISIONS (S_IRWXU | S_IRWXG | S_IRWXO)
#define USERNAME_LENGTH 16
#define PASSWORD_LENGTH 24
#define SSRC_LENGTH 8
#define CNAME_LENGTH 16
#define MSLABEL_LENGTH 36
#define TEMPLATE_STRING "TEMPLATE_STRING"
#define MAX_EVENT_POOL_THREADS 5
//hash map for conference entries & related functions
typedef ACE_Hash_Map_Manager<ACE_TString, xGateHmpConference*,  ACE_SYNCH_RW_MUTEX> HASH_HMP_CONF_MAP;
typedef std::unordered_map<string, xGateHmpConference*> HMP_CONF_MAP;

//hash map for frameBin & client map
typedef ACE_Hash_Map_Manager<void *, Client*,  ACE_SYNCH_RW_MUTEX> HASH_HMP_FRAMEBIN_CLIENT_MAP;

class SFUController
{
public:
  SFUController(const char*);
  SFUController()
  {

  }
  virtual ~SFUController(void);
  /**
  * Initialize the Hmp Gstreamer Manager class and thread
  */
  xGateRetVal init(void);
  /**
  * Start the event loop for processing messages/notifications. This method
  * must be static in order to start event loop processing in a newly spawned thread.
  * Also, this is an infinitely blocking call.
  */
  static void run(void* arguments);
  /**
  *  stopThread( void );
  */
  xGateRetVal stopThread( void );
  xGateRetVal release_channel(MgMediaDetail &mediaDetail);
  xGateRetVal decode_media_msg(xGateMgMsg *pMgMsg);
  xGateRetVal SFUController::set_media_details(MgMediaDetail &mediaDetail, ClientDetail &clientDetail);
  xGateRetVal media_delete_channel(MgMediaDetail &mediaDetail);
  xGateRetVal add_participant(MgMediaDetail &mediaDetail);
  xGateRetVal handle_sdp_update_response_msg(MgMediaDetail &mediaDetail);
  xGateRetVal handle_sdp_update_request_msg(MgMediaDetail &mediaDetail);
  xGateRetVal handle_tile_change_request_msg(MgMediaDetail &mediaDetail);
  xGateRetVal handle_screenshare_request_msg(MgMediaDetail &mediaDetail);
  xGateRetVal handle_active_speaker_request_msg(MgMediaDetail &mediaDetail);
  xGateRetVal handle_grid_view_request_msg (MgMediaDetail &mediaDetail);
  xGateRetVal allocate_conf_channel(MgMediaDetail &mediaDetail, xGateNetConnection &pbxConn);
  xGateRetVal switch_callto_conference(MgMediaDetail &mediaDetail);
  xGateRetVal insert_conference_entry(string conferenceId, xGateHmpConference* pHmpConference);
  xGateRetVal remove_conference_entry(string conferenceId);
  static void* gen_offers(void*);
  static void  send_msg_clients_gen_offers(void*);
  xGateRetVal send_msg_sfu_client(MgMediaDetail *,ClientDetail & ,xGateMgMsgType eMsgType);
  xGateHmpConference* find_conference_entry(string &conferenceId);
  Client* find_client_entry(HmpFrameBin *pFrameBin);
  void send_key_frame_request(MgMediaDetail &mediaDetail);
  void send_create_send_channel_request (MgMediaDetail &mediaDetail);
  void handle_conference_release (MgMediaDetail &mediaDetail);
  void retry_media_connection_request (MgMediaDetail &mediaDetail);
#ifdef RECORD_ENABLED
  xGateRetVal handle_record_event_req(MgMediaDetail &mediaDetail);
  xGateRetVal handle_record_event_res(MgMediaDetail &mediaDetail);
  xGateRetVal handle_record_keyframe_request(MgMediaDetail &mediaDetail);
  SFUClientRcvdVideoHandler* get_rcvd_video_handler();
  SFUClientRcvdAudioHandler* get_rcvd_audio_handler();
  SFUClientRecordVideoHandler* get_record_video_handler();
  SFUClientRecordAudioHandler* get_record_audio_handler();
  xGateHmpConference *m_hmpConf;
#endif
  ThreadPool  m_ThreadPool_info;
private:

  /**
  * Private copy constructor - disallow copies and automatic methods.
  **/
  SFUController(const SFUController& rhs);

  /**
  * Private assignment operator - disallow assignment and automatic methods.
  **/
  SFUController& operator= (const SFUController& rhs);

  /**
  * Init Gstreamer Library
  */
  xGateRetVal initGstreamerLib();
  bool m_running;
  bool m_gst_init_complete;
  char m_serverType[SERVER_TYPE_SIZE];
 //for Video
  GMainLoop *gMainLoop;

#ifdef RECORD_ENABLED
  SFUClientRcvdVideoHandler *m_rcvd_video_handler;
  SFUClientRcvdAudioHandler *m_rcvd_audio_handler;
  SFUClientRecordVideoHandler *m_record_video_handler;
  SFUClientRecordAudioHandler *m_record_audio_handler;
  gboolean spawn_media_handler_pool();
#endif

  //hash map for call entries & related functions
  //HASH_HMP_CONF_MAP m_hmpConfMap;
  HMP_CONF_MAP m_hmpConfMap;
  xGateRetVal create_doc_object(Document &doc, const char *jsonData, Value& dataMemObj);
};

#endif // _SFU_CONTROLLER_
