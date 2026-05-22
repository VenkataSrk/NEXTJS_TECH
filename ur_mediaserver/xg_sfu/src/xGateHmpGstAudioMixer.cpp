//local include

#include <time.h>
#include "xGateHmpGstBin.h"
#include "xGateHmpGstUtil.h"
#include "xGateHmpGstVideoBin.h"
#include <gst/gstbuffer.h>
#include "xglog.h"
#include "xGateSFUtil.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <cmath>
#include <cstdlib>
#include <thread>
#define THISMODULE "AudioMixer"
#define FILE_PATH "/root/"
using namespace std::chrono;
typedef unsigned char orc_uint8;
typedef signed char orc_int8;
typedef short  orc_int16;
typedef union { orc_int16 i; orc_int8 x2[2]; } orc_union16;

#define ORC_CLAMP(x,a,b) ((x)<(a) ? (a) : ((x)>(b) ? (b) : (x)))
#define ORC_ABS(a) ((a)<0 ? -(a) : (a))
#define ORC_MIN(a,b) ((a)<(b) ? (a) : (b))
#define ORC_MAX(a,b) ((a)>(b) ? (a) : (b))
#define ORC_SB_MAX 127
#define ORC_SB_MIN (-1-ORC_SB_MAX)
#define ORC_UB_MAX (orc_uint8) 255
#define ORC_UB_MIN 0
#define ORC_SW_MAX 32767
#define ORC_SW_MIN (-1-ORC_SW_MAX)
#define ORC_UW_MAX (orc_uint16)65535
#define ORC_UW_MIN 0
#define ORC_SL_MAX 2147483647
#define ORC_SL_MIN (-1-ORC_SL_MAX)
#define ORC_UL_MAX 4294967295U
#define ORC_UL_MIN 0

#define ORC_CLAMP_SB(x) ORC_CLAMP(x,ORC_SB_MIN,ORC_SB_MAX)
#define ORC_CLAMP_UB(x) ORC_CLAMP(x,ORC_UB_MIN,ORC_UB_MAX)
#define ORC_CLAMP_SW(x) ORC_CLAMP(x,ORC_SW_MIN,ORC_SW_MAX)
#define ORC_CLAMP_UW(x) ORC_CLAMP(x,ORC_UW_MIN,ORC_UW_MAX)
#define ORC_CLAMP_SL(x) ORC_CLAMP(x,ORC_SL_MIN,ORC_SL_MAX)
#define ORC_CLAMP_UL(x) ORC_CLAMP(x,ORC_UL_MIN,ORC_UL_MAX)


xGateRetVal Client::spawn_audio_mixer ()
{
   char *thr_name = "AudioMixer";
   Client  *client = this;
   ACE_thread_t *thr_id = &client->audio_mixer_thrId;
   if (ACE_Thread::spawn(this->mix_participants_audio,
                            this,
                            THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, //THR_SCHED_RR
                            thr_id,
                            0,
                            0, /* priority */
                            0,
                            ACE_DEFAULT_THREAD_STACKSIZE,
                            0,
                            &thr_name) == -1)
    {
      XGLOG_ERROR("Client:: spawn_audio_mixer() failed to start client thread !");
      return EN_XGATE_STATUS_ERROR;
    }
    else
    {
       XGLOG_INFO("Client::spawn_audio_mixer () Spawning thread success");
    }
    return EN_XGATE_STATUS_SUCCESS;
}

void Client::mix_participants_audio (void *pVoidObj)
{
  xGateHmpConference   *pHmpConfObj = nullptr;
  Client *client = (Client*) pVoidObj;
  gulong wait_time_ms = 0;
  gulong start_time = 0;
  gulong end_time = 0;
  HmpAudioEncoderBin *AudioEncoderBin = NULL;
  GstElement *rtpfunnel_element = NULL;
  string str_file_name = "";
  GstBuffer *out_buffer = NULL;
  guint8   ucThrName [16] = {'\0'};
  guint8   ucCallId [10] = {'\0'};
  GstClockTime  clocktime = GST_CLOCK_TIME_NONE;
  GstClockTime  clocktimediff = 0;
  ACE_Message_Block* pAmb = NULL;
  xGateHmpClientThreadMsg *pMsg = NULL;
  guint size = 0;
  size_t num_msg = 0;
  guint  num_vect_msg = 0;
  GstMapInfo out_map;
  GstMapInfo  in_map;
  GstBuffer *in_buffer = NULL;
  mlineofferDetail *mlineDetails = nullptr;
  vector <Client*> vAudMixedClient;

  SFUController *pSFUController = xGateSFUtil::getSFUController();
  pHmpConfObj = pSFUController->find_conference_entry(client->m_detail.m_mgresourceId);
  if (!pHmpConfObj){
    XGLOG_ERROR ("Failed to get pHmpConfObj for mgresourceId: %s client: %s",
          client->m_detail.m_mgresourceId.c_str(), client->m_detail.m_callId.c_str());
  }
  XGLOG_INFO ("client: %s Entered mix_participants_audio () thread", client->m_detail.m_callId.c_str());
  if(!(client->mixer_msg_q
        = new ACE_Message_Queue <ACE_MT_SYNCH, ACE_System_Time_Policy > ())){
    XGLOG_ERROR("Error in initializating audio mixer message queue ");
    return;
  }
  snprintf (ucCallId, 9, "%s", client->m_detail.m_callId.c_str());
  sprintf (ucThrName,"%s_%s", "Mixer", ucCallId);
  XGLOG_INFO ("setting thread name: %s", ucThrName);
  SET_THREAD_NAME (pthread_self(), ucThrName);

  // sleep till Encoderbin and offer details are created
  do
  {
     if (client->m_FrameChannel){
       AudioEncoderBin = client->m_FrameChannel->m_pHmpAudioEncoderBin;
     }
     if (!rtpfunnel_element)
       rtpfunnel_element = gst_bin_get_by_name (GST_BIN (client->m_FrameChannel->m_pipeline), "rtpfunnel");

     if (AudioEncoderBin && client->m_mlineOffer_vector.size() && rtpfunnel_element)
       break;
    usleep (MIXER_WAIT_TIME_USEC);
  } while (client->m_mlineOffer_vector.size() && AudioEncoderBin && rtpfunnel_element);

  if (rtpfunnel_element)
    gst_object_unref (rtpfunnel_element);

  mlineDetails = client->m_mlineOffer_vector.at (0);

  XGLOG_INFO ("client: %s mlineDetails: %p rtpfunnel_element: %p AudioEncoderBin: %p",
              client->m_detail.m_callId.c_str(), mlineDetails, rtpfunnel_element, AudioEncoderBin);
  // wait till we are ready to Tx data
  do {
    if (mlineDetails && TRUE == mlineDetails->audioOfferDetails.bSendPkt)
      break;
    // flush the buffers till we are ready to transmit the data
    num_msg = client->mixer_msg_q->message_count ();
    for (guint uiIndex = 0; uiIndex < num_msg; uiIndex++){
      ACE_Message_Block* pAmb = NULL;
      int result = client->mixer_msg_q->dequeue_head(pAmb, NULL);
      if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
      {
        XGLOG_WARN ("no data in msgQ result: %d errno: %d", result, errno);
        continue;
      }
      delete pAmb;
      pAmb = NULL;
    }
    usleep (MIXER_WAIT_TIME_USEC);
  } while (!mlineDetails && mlineDetails->audioOfferDetails.bSendPkt != TRUE);

  AudioEncoderBin->lastAudioMixedTime = gst_element_get_current_running_time (rtpfunnel_element);

  XGLOG_INFO ("client: %s bSendPkt: %u bDtxEnabled: %u ",
         client->m_detail.m_callId.c_str(), mlineDetails->audioOfferDetails.bSendPkt,
         AudioEncoderBin->bDtxEnabled);

  while (FALSE == client->m_StopSendRecvData) {

    // check the no of msg in Q
    gboolean bIsBufferMixed = FALSE;
    num_msg = 0;
    int result = client->mixer_msg_q->dequeue_head(pAmb, NULL);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
       XGLOG_WARN ("client: %s no data in msgQ result: %d errno: %d",
                client->m_detail.m_callId.c_str(), result, errno);
       continue;
    }

    start_time = clocktime = gst_element_get_current_running_time (rtpfunnel_element);

    if (pAmb)
      client->vInpBuffersToMix.push_back (pAmb);
    num_msg = client->mixer_msg_q->message_count ();

    for (guint uiIndex = 0; uiIndex < num_msg; uiIndex++){
      ACE_Message_Block* pAmb = NULL;
      int result = client->mixer_msg_q->dequeue_head(pAmb, NULL);
      if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
      {
        XGLOG_WARN ("no data in msgQ result: %d errno: %d", result, errno);
        continue;
      }
      client->vInpBuffersToMix.push_back (pAmb);
    }

   #ifdef AUDIO_MIXER_FILE_DUMP
    if (((pHmpConfObj->m_clientMap.size() - 1 >= 2)
        || (pHmpConfObj->m_recorderMap.size() > 0))
          && (num_msg > 0)) {
      DumpAudioData (client, DUMP_MIXER_OUTPUT_DATA, nullptr, 0, nullptr);
    }
   #endif

    num_vect_msg = client->vInpBuffersToMix.size();

    // Allocate output buffer in 3 cases
    // 1. if there is atleast one buffer to be mixed
    // 2. if there are no buffers in Q and DTX is NOT enabled, then we need to send
    //    silence data. The output of the mixer is silence audio in this case
    // 3. wait for 100 ms before deciding to send silence samples as there may be
    //    buffers put in Q in next cycle and if send silence , it results in audio
    //    glitch. compare lastAudioMixed time with current time and if not data is mixed
    //    for a certian duration, send silence

    XGLOG_TRACE ("client: %s bDtxEnabled: %u num_vect_msg: %u",
             client->m_detail.m_callId.c_str(), AudioEncoderBin->bDtxEnabled, num_vect_msg);

process_again:
    if (start_time > AudioEncoderBin->lastAudioMixedTime)
      clocktimediff =  start_time - AudioEncoderBin->lastAudioMixedTime;
    else
      clocktimediff = AudioEncoderBin->lastAudioMixedTime - start_time;

    clocktimediff /= NANO_TO_MILLI_SEC;

    gboolean bIsDataAvail = FALSE;
    if (num_vect_msg > 0) {
      bIsDataAvail = client->peek_data_avail_participants ();
    }
    XGLOG_TRACE ("AFTER calling peek_data_avail_participants () bIsDataAvail: %u clocktimediff: %u MAX_LATENCY_MS: %u",
                            bIsDataAvail, clocktimediff, MAX_LATENCY_MS);
    if ((bIsDataAvail == FALSE) && (clocktimediff <= MAX_LATENCY_MS)) {
      XGLOG_TRACE ("bIsDataAvail: %u clocktimediff: %u  waiting... ", bIsDataAvail, clocktimediff);
      goto WAIT;
    }

    out_buffer = NULL;
    if (num_vect_msg > 0 && (clocktimediff >= MAX_LATENCY_MS || (bIsDataAvail == TRUE))) {
      size = AUDIO_SAMPLES_16K_20MS * BYTES_PER_SAMPLE_16BIT;
      out_buffer = gst_buffer_new_allocate (NULL, size, NULL);
      gst_buffer_map (out_buffer, &out_map, GST_MAP_READWRITE);
    //if (strncmp(client->m_detail.m_callId.c_str(), "201", 3) == 0)
      XGLOG_TRACE ("client: %s allocated out_map.data: %p size: %d clocktimediff: %lld num_vect_msg mixer Q: %d",
        client->m_detail.m_callId.c_str(), out_map.data, out_map.size, clocktimediff, num_vect_msg);
      memset (out_map.data, 0, size);
      bIsBufferMixed = TRUE;
    } else {
      XGLOG_TRACE ("client: %s NOT allocated out mem as num_vect_msg: %u and clockdiff: %lld < 100ms",
                     client->m_detail.m_callId.c_str(), num_vect_msg, clocktimediff);
      goto WAIT;
    }

    for (int index = 0; index < num_vect_msg; index++) {
      pAmb = client->vInpBuffersToMix.at (index);
      pMsg = static_cast<xGateHmpClientThreadMsg*>(pAmb);
      in_buffer = NULL;
      switch (pMsg->m_msgType){
        case XGATE_CLIENT_MIX_AUDIO:
        {
          gchar *element_name = static_cast <char *> (pMsg->ucName);
          Client *src_client = static_cast <Client *> (pMsg->param3);
          guint       uiMixedIndex = 0;
          gboolean    bAlreadyMixed = FALSE;
          in_buffer   = (GstBuffer *) pMsg->param2;

          if ((src_client->m_detail.m_callId == client->m_detail.m_callId)
             && (pHmpConfObj->m_clientMap.size() - 1 > 1)){
            XGLOG_ERROR ("src_client: %s == client: %s erasing at index: %u continue..",
                       src_client->m_detail.m_callId.c_str(),
                       client->m_detail.m_callId.c_str(), index );
            client->vInpBuffersToMix.erase (client->vInpBuffersToMix.begin() + index);
            num_vect_msg = client->vInpBuffersToMix.size();
            delete pAmb;
            pAmb = NULL;
            continue;
          }

          XGLOG_TRACE ("Mixing samples from client: %s msg: %p num_vect_msg: %u index: %u",
                       src_client->m_detail.m_callId.c_str(), pAmb, num_vect_msg, index);

          for (uiMixedIndex = 0; uiMixedIndex < vAudMixedClient.size(); uiMixedIndex++){
            if (vAudMixedClient.at(uiMixedIndex) == src_client) {
              bAlreadyMixed = TRUE;
              break;
            }
          }
          if (TRUE == bAlreadyMixed){
            XGLOG_TRACE ("client: %s Already mixed ,,Queing msg: %p to process in next iteration index: %u",
                         src_client->m_detail.m_callId.c_str(), pAmb, index);
            bAlreadyMixed = FALSE;
            continue;
          }
          vAudMixedClient.push_back (src_client);

          if (TRUE != gst_buffer_map (in_buffer, &in_map, GST_MAP_READ)){
            XGLOG_ERROR ("src_client: %s Failed to map input buffer",
                          src_client->m_detail.m_callId.c_str());
            bAlreadyMixed = FALSE;
            gst_buffer_unref (in_buffer);
            continue;
          }
          /* Buffers of same client should not be mixed in one iteration.
           * Find if there are multiple buffers of the same client that need to
           * be mixed, it should be done in next iteration i,,e after one frame
           * duration.
           *
           * if we find such buffers in queue, post the buffer in queue so that those
           * buffers will be mixed in next iteration
           */

       #ifdef AUDIO_MIXER_FILE_DUMP
          client->DumpAudioData (client, DUMP_MIXER_INPUT_DATA, in_map.data, in_map.size, src_client);
       #endif
          client->add_buffers (src_client,
                            element_name, in_map.data, out_map.data, in_map.size, out_map.size);
          bIsBufferMixed = TRUE;
          gst_buffer_unmap (in_buffer, &in_map);
          gst_buffer_unref (in_buffer);

          clocktime = gst_element_get_current_running_time (rtpfunnel_element);
          AudioEncoderBin->lastAudioMixedTime = clocktime;

          client->vInpBuffersToMix.erase (client->vInpBuffersToMix.begin() + index);
          num_vect_msg = client->vInpBuffersToMix.size();
          XGLOG_TRACE ("client: %s audio of client %s is mixed,, erased index: %u remaining num_vect_msg: %u",
                   client->m_detail.m_callId.c_str(), src_client->m_detail.m_callId.c_str(),
                   index, num_vect_msg);
          index--;
          delete pAmb;
          pAmb = NULL;
        }
        break;
        case XGATE_CLIENT_THREAD_STOP:
        {
          XGLOG_INFO ("received THREAD_STOP msg for AudioMixer ..Exiting from loop");
          delete pAmb;
          pAmb = NULL;
          flush_msg_queue (client->mixer_msg_q);
          goto flush_msg_q;
        }
        break;
        default:
          XGLOG_WARN ("Unknown message sent to mixer thread");
          break;
      }

      if (pAmb)
        delete pAmb;
      pAmb = NULL;
    } // end of for loop

    // erase the vector
    if (vAudMixedClient.size()){
      vAudMixedClient.clear();
    }
    if (out_buffer ){
       #ifdef AUDIO_MIXER_FILE_DUMP
         DumpAudioData (client, DUMP_MIXER_OUTPUT_DATA, out_map.data, out_map.size, nullptr);
      #endif
       gst_buffer_unmap (out_buffer, &out_map);
       if ((AudioEncoderBin ) && (TRUE == bIsBufferMixed)) {
          xGateHmpClientThreadMsg *pMsg = NULL;
          GError *error = nullptr;
          GstElement *element = AudioEncoderBin->m_opusenc_filter;
            GstClockTime duration = 0;
            //if (strncmp(client->m_detail.m_callId.c_str(), "201", 3) == 0)
            XGLOG_TRACE ("client: %s audio is mixed pushing to encoder @clocktime: %lld clocktimediff: %llu",
                             client->m_detail.m_callId.c_str(), clocktime, clocktimediff);
            GST_BUFFER_DTS (out_buffer) = GST_CLOCK_TIME_NONE;
            GST_BUFFER_PTS (out_buffer) = clocktime;
            duration = SAMPLE_DURATION_20MS * NANO_TO_MILLI_SEC;
            GST_BUFFER_DURATION (out_buffer) = duration;
            GST_BUFFER_OFFSET (out_buffer) = 0;
            GST_BUFFER_OFFSET_END (out_buffer) = 0;
            GST_BUFFER_FLAG_SET (out_buffer, GST_BUFFER_FLAG_GAP);

            pMsg = new xGateHmpClientThreadMsg ();
            pMsg->m_msgType = XGATE_CLIENT_ENCODE_AUDIO;
            pMsg->param2 = static_cast <void*> (out_buffer);
            pMsg->pClient = client;

            // Take reference of buffer and unref after encoding to avoid race
            // condition of buffer getting unreffed before the buffer gets encoded
            gst_buffer_ref (out_buffer);
            g_mutex_lock (&client->m_audio_encode_process_lock);

            client->ullNextAudioEncodeMsgNum++;
            pMsg->ullMsgNumber = client->ullNextAudioEncodeMsgNum;

            g_thread_pool_push (pSFUController->m_ThreadPool_info.m_encoder_pool, pMsg, &error);
            if (error != nullptr){
               XGLOG_ERROR ("Failed to push buffer to encoder thread pool client: %s",
                client->m_detail.m_callId.c_str());
               client->ullNextAudioEncodeMsgNum--;
               g_error_free (error);
            }
            g_mutex_unlock (&client->m_audio_encode_process_lock);
       } else {
         gst_buffer_unref (out_buffer);
       }
    }

    num_vect_msg = client->vInpBuffersToMix.size();
    if (num_vect_msg > 0)
      goto process_again;

WAIT:

    end_time = gst_element_get_current_running_time (rtpfunnel_element);

    wait_time_ms =  end_time - start_time;
    //convert to micro secs
    wait_time_ms /= 1000;

    XGLOG_TRACE ("client: %s time spent to mix audio: %u micro_sec",
                                 client->m_detail.m_callId.c_str(), wait_time_ms);
    if (wait_time_ms < MIXER_WAIT_TIME_USEC)
      wait_time_ms = MIXER_WAIT_TIME_USEC - wait_time_ms;
    else
      wait_time_ms = MIXER_WAIT_TIME_USEC;
    // wake up for every 20 msec to mix audio of different participants
    XGLOG_TRACE ("client: %s next wakeup time: %u milli_sec num_buffers in vector: %u",
                  client->m_detail.m_callId.c_str(),wait_time_ms, client->vInpBuffersToMix.size());
    //usleep (wait_time_ms);
  } // end of while loop
flush_msg_q:
  client->mixer_msg_q->flush();
  client->mixer_msg_q->close();
  delete client->mixer_msg_q;
  client->mixer_msg_q = NULL;
  client->vInpBuffersToMix.clear();
  XGLOG_INFO ("Exiting mix_participants_audio() ");
}

void Client::add_buffers (Client *src_client, gchar *element_name,
                          gpointer inp, gpointer out, guint in_size, guint out_size)
{
  out_size = (out_size > in_size) ? in_size : out_size;

  orc_union16 *ptr0;
  const orc_union16 *ptr4;

  orc_union16 var32;
  orc_union16 var33;
  orc_union16 var34;

  ptr0 = (orc_union16 *) out;
  ptr4 = (orc_union16 *) inp;


  for (guint i = 0; i < out_size / BYTES_PER_SAMPLE_16BIT; i++) {
    /* 0: loadw */
    var32 = ptr0[i];
    /* 1: loadw */
    var33 = ptr4[i];
    /* 2: addssw */
    var34.i = ORC_CLAMP_SW (var32.i + var33.i);
    /* 3: storew */
    ptr0[i] = var34;

  }

}

gboolean Client::peek_data_avail_participants ()
{
   Client *client = this;
   Client *audio_client = nullptr;
   Client *tmpClient = nullptr;
   xGateHmpConference   *pHmpConfObj = nullptr;
   guint32  num_vect_msg = 0;
   guint32 uiIndex = 0;
   guint32   uiNumAudioParticipantsConf = 0;
   guint32   uiNumAudioPart = 0;
   ACE_Message_Block  *pAmb = nullptr;
   xGateHmpClientThreadMsg  *pMsg = nullptr;

   SFUController *pSFUController = xGateSFUtil::getSFUController();

   pHmpConfObj = pSFUController->find_conference_entry(client->m_detail.m_mgresourceId);
   if (!pHmpConfObj){
     XGLOG_ERROR ("Failed to get pHmpConfObj for mgresourceId: %s client: %s",
          client->m_detail.m_mgresourceId.c_str(), client->m_detail.m_callId.c_str());
     return TRUE;
   }

   num_vect_msg = client->vInpBuffersToMix.size();
   guint32 num_clients = pHmpConfObj->m_clientVector.size ();
   XGLOG_TRACE ("client: %s Entered peek_data_avail_participants () clientVector.size: %u num_vect_msg: %u",
                client->m_detail.m_callId.c_str(), num_clients , num_vect_msg);
   // check if data is available from all the participants. wait till data is available from
   // all the participants to mix
   for (guint uiConfIndex = 0; uiConfIndex < pHmpConfObj->m_clientVector.size(); uiConfIndex++) {
     tmpClient = pHmpConfObj->m_clientVector.at (uiConfIndex);
     if (tmpClient && tmpClient->m_FrameChannel
        && (0 != strcmp (tmpClient->m_detail.m_callId.c_str(), DUMMY_CALLID)
        && (tmpClient->m_detail.m_callId != client->m_detail.m_callId))) {
        HmpFrameBin *pHmpFrameBin = tmpClient->m_FrameChannel->m_pHmpFrameBin;
        if ((pHmpFrameBin && pHmpFrameBin->m_negotiated_audio_payload_num > 0) &&
            (TRUE == pHmpFrameBin->bStartedRecvAudio)) {
          uiNumAudioParticipantsConf++;
          for (uiIndex = 0; uiIndex < num_vect_msg; uiIndex++) {
            pAmb = client->vInpBuffersToMix.at (uiIndex);
            pMsg = static_cast<xGateHmpClientThreadMsg*>(pAmb);
            if (pMsg->param3 == tmpClient){
              uiNumAudioPart++;
              break;
            }
         }
       }
     }
     tmpClient = nullptr;
   }

   XGLOG_TRACE ("client: %s uiNumAudioPart: %u uiNumAudioParticipantsConf: %u",
                     client->m_detail.m_callId.c_str(), uiNumAudioPart, uiNumAudioParticipantsConf);
   if (uiNumAudioPart == uiNumAudioParticipantsConf)
     return TRUE;
   else
     return FALSE;
}

#ifdef AUDIO_MIXER_FILE_DUMP
void Client::DumpAudioData (Client *client, guint32 u32Type, gchar *i8Data,
                            guint32 u32size , Client *src_client)
{
  string               str_file_name = "";
  xGateHmpConference   *pHmpConfObj = nullptr;
  gint                 bytes_wrote = 0;
  SFUController *pSFUController = xGateSFUtil::getSFUController();

  pHmpConfObj = pSFUController->find_conference_entry(client->m_detail.m_mgresourceId);
  if (!pHmpConfObj){
    XGLOG_ERROR ("Failed to get pHmpConfObj for mgresourceId: %s client: %s",
          client->m_detail.m_mgresourceId.c_str(), client->m_detail.m_callId.c_str());
  }

  if ((u32Type == DUMP_MIXER_OUTPUT_DATA) &&
       !client->fp && pHmpConfObj->m_webrtcCount >= 3
       && (0 == strncmp (client->m_detail.m_callId.c_str(), "201", 3))){
    str_file_name += FILE_PATH;
    str_file_name += "AudMix_out_dump_";
    str_file_name += client->m_detail.m_callId.c_str();
    str_file_name += ".raw";
    XGLOG_INFO ("client: %s opening audio mixer file dump %s" ,
                           client->m_detail.m_callId.c_str(), str_file_name.c_str());
    client->fp = fopen (str_file_name.c_str(), "w+");
    str_file_name.clear();

    client->in_fp1 = NULL;
    str_file_name = FILE_PATH;
    str_file_name += "Audio_In_dump_Guest.raw";
    client->in_fp1 = fopen (str_file_name.c_str(), "w+");
    str_file_name.clear ();

    client->in_fp2 = NULL;
    str_file_name = FILE_PATH;
    str_file_name += "Audio_In_dump_screenshare.raw";
    client->in_fp2 = fopen (str_file_name.c_str(), "w+");
    str_file_name.clear ();
  }

  if (client->fp && i8Data && (u32Type == DUMP_MIXER_OUTPUT_DATA)) {
    bytes_wrote = fwrite (i8Data, 1, u32size, client->fp);
    if (u32size != bytes_wrote)
      XGLOG_ERROR ("Failed to dump mixer audio expected write: %u wrote: %u",
                           u32size, bytes_wrote);
  }
  if (client->in_fp1 && (u32Type == DUMP_MIXER_INPUT_DATA) && i8Data
    && (0 == strncmp (src_client->m_detail.m_callId.c_str(), "G_3390", 6))
    && (src_client->m_detail.m_joinerType != EN_JOINER_TYPE_SCREENSHARE )) {
    bytes_wrote = fwrite (i8Data, 1, u32size, client->in_fp1);
    if (u32size != bytes_wrote)
      XGLOG_ERROR ("Failed to dump mixer audio expected write: %u wrote: %u",
                           u32size, bytes_wrote);
  } else if (client->in_fp2 && (u32Type == DUMP_MIXER_INPUT_DATA) && i8Data
              && (src_client->m_detail.m_joinerType == EN_JOINER_TYPE_SCREENSHARE)) {
    bytes_wrote = fwrite (i8Data, 1, u32size, client->in_fp2);
    if (u32size != bytes_wrote)
      XGLOG_ERROR ("Failed to dump mixer audio expected write: %u wrote: %u",
                           u32size, bytes_wrote);
  }

  if ((u32Type == DUMP_DECDODER_DATA)
     && !client->dec_fp1 && pHmpConfObj->m_webrtcCount >= 3
     && (0 == strncmp (client->m_detail.m_callId.c_str(), "201", 3))
     && src_client && (0 == strncmp (src_client->m_detail.m_callId.c_str(), "G_3390", 6))
     && (src_client->m_detail.m_joinerType != EN_JOINER_TYPE_SCREENSHARE)){
    str_file_name = "";
    str_file_name += FILE_PATH;
    str_file_name += "Decoded_dump_G_3390_Guest_201.raw";
    XGLOG_INFO ("client: %s opening Decoder dump file dump for %s" ,
                           client->m_detail.m_callId.c_str(), str_file_name.c_str());
    client->dec_fp1 = fopen (str_file_name.c_str(), "w+");
    str_file_name.clear();
  }

  if ((u32Type == DUMP_DECDODER_DATA)
    && !client->dec_fp2 && pHmpConfObj->m_webrtcCount >= 3
     && (0 == strncmp (client->m_detail.m_callId.c_str(), "201", 3))
    && src_client && (0 == strncmp (src_client->m_detail.m_callId.c_str(), "G_3390", 6))
    && (src_client->m_detail.m_joinerType == EN_JOINER_TYPE_SCREENSHARE)) {
     str_file_name += FILE_PATH;
     str_file_name += "Decoded_dump_G_3390_Guest_screenshare.raw";
     XGLOG_INFO ("client: %s opening Decoder dump file dump for %s" ,
                           client->m_detail.m_callId.c_str(), str_file_name.c_str());
     client->dec_fp2 = fopen (str_file_name.c_str(), "w+");
     str_file_name.clear();
   }

   if ((u32Type == DUMP_DECDODER_DATA) && (0 == strncmp (client->m_detail.m_callId.c_str(), "201", 3))) {
     if (client->dec_fp1 && (0 == strncmp (src_client->m_detail.m_callId.c_str(), "G_3390", 6))
           && (src_client->m_detail.m_joinerType != EN_JOINER_TYPE_SCREENSHARE)){
       bytes_wrote = fwrite (i8Data, 1, u32size, client->dec_fp1);
       if (u32size != bytes_wrote)
         XGLOG_ERROR ("Failed to dump decoded audio expected write: %u wrote: %u",
                           u32size, bytes_wrote);
     } else if (client->dec_fp2 && (0 == strncmp (src_client->m_detail.m_callId.c_str(), "G_3390", 6))){
       bytes_wrote = fwrite (i8Data, 1, u32size, client->dec_fp2);
       if (u32size != bytes_wrote)
         XGLOG_ERROR ("Failed to dump decoded audio expected write: %u wrote: %u",
                           u32size, bytes_wrote);
    }
  }
}
#endif
