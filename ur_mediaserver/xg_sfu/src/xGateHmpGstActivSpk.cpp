//local include

//ace include
#include <ace/Task.h>
#include <time.h>
#include<mutex>
//self include
#include "xGateHmpGstBin.h"
#include "xGateHmpGstUtil.h"
#include "xGateHmpGstVideoBin.h"
#include <gst/sdp/sdp.h>
#include <gst/webrtc/webrtc.h>
#include <gst/webrtc/webrtc_fwd.h>
#include <gst/gstbuffer.h>
#include <gst/rtp/gstrtpbuffer.h>
#include <gst/rtp/gstrtcpbuffer.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include "xglog.h"
#include "xGateSFUtil.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <cmath>
#include <cstdlib>
#define THISMODULE "ActivSpkr"
//#define LOG_ACTIVE_SPK_INFO
void Client::handle_audio_level_msg (Client *client, GstStructure *level_struct)
{
  gdouble rms_dB = 0;
  gdouble peak_dB = 0;
  gdouble decay_dB = 0;
  gdouble rms = 0;
  AudioSampleInfo sampleInfo;
  GValueArray *rms_arr, *peak_arr, *decay_arr;
  const GValue *array_val = NULL;
  gint channels = 0;
  gint index = 0;
  const GValue *value = NULL;
  //XGLOG_INFO ("Entered handle_audio_level_msg () for client: %s ",
  //             client->m_detail.m_callId.c_str());

  array_val = gst_structure_get_value (level_struct, "rms");
  rms_arr = (GValueArray *) g_value_get_boxed (array_val);

  array_val = gst_structure_get_value (level_struct, "peak");
  peak_arr = (GValueArray *) g_value_get_boxed (array_val);

  array_val = gst_structure_get_value (level_struct, "decay");
  decay_arr = (GValueArray *) g_value_get_boxed (array_val);

  channels = rms_arr->n_values;
  client->m_sampleInfo_mutex.lock();
  for (index = 0; index < channels; index++){

     value = g_value_array_get_nth (rms_arr, index);
     rms_dB = g_value_get_double (value);

     value = g_value_array_get_nth (peak_arr, index);
     peak_dB = g_value_get_double (value);

     value = g_value_array_get_nth (decay_arr, index);
     decay_dB = g_value_get_double (value);

    /* converting from dB to normal gives us a value between 0.0 and 1.0 */
     rms = pow (10, rms_dB / 20);
     gdouble peak = pow (10, peak_dB / 20);
     gdouble decay = pow (10, decay_dB/ 20);

     client->m_sample_info[client->m_last_index].rms_dB = rms_dB;
     client->m_sample_info[client->m_last_index].peak_dB = peak_dB;
     client->m_sample_info[client->m_last_index].decay_dB = decay_dB;

     XGLOG_TRACE("client: %s m_last_index: %d RMS: %f dB, peak: %f dB, decay: %f dB After Normalizing,, rms: %f peak: %f decay: %f",
            client->m_detail.m_callId.c_str(),client->m_last_index,
            client->m_sample_info[client->m_last_index].rms_dB,
            client->m_sample_info[client->m_last_index].peak_dB,
            client->m_sample_info[client->m_last_index].decay_dB, rms, peak, decay);
      client->m_last_index++;
      client->m_last_index %= MAX_NUM_SAMPLES;
  }
  client->m_sampleInfo_mutex.unlock();
}

xGateRetVal xGateHmpConference::spawn_active_spkr_detect ()
{
   char *thr_name = "active_spkr_thr";
   if (ACE_Thread::spawn(this->detect_active_spkr_participant,
                            this,
                            THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, //THR_SCHED_RR
                            0,
                            0,
                            0, /* priority */
                            0,
                            ACE_DEFAULT_THREAD_STACKSIZE,
                            0,
                            &thr_name) == -1)
    {
      XGLOG_ERROR("xGateHmpConference:: failed to start client thread !");
      return EN_XGATE_STATUS_ERROR;
    }
    else
    {
       XGLOG_INFO("xGateHmpConference:: Spawning thread success");
    }
    return EN_XGATE_STATUS_SUCCESS;
}

void xGateHmpConference::detect_active_spkr_participant (void *pVoidObj)
{
  xGateHmpConference   *pHmpConfObj = (xGateHmpConference *) pVoidObj;
  Client *dst_client = nullptr;
  string  new_active_spkr_callId = "";
  gdouble avg_rms = 0;
  gdouble avg_peak_variation = 0;
  gdouble  avg_decay_variation = 0;
  AUDIO_SAMPLE_INFO  sSampleInfo;
  std::vector <AUDIO_SAMPLE_INFO> sSortSampleInfo;
  gdouble rms_variation = 0;
  gdouble peak_variation = 0;
  guint   current_actspkr_index = 0;
  XGLOG_INFO ("Entered detect_active_spkr_participant thread");

  SET_THREAD_NAME (pthread_self(), "ActSpkr_");
  while (FALSE == pHmpConfObj->m_StopConference) {
    // wake up for every 1 sec to check the active speaker participant
    // need to be analyzed if lesser duration need to selected
    usleep (WAIT_TIME_USEC);

    // if there is single participant in conference , no active speaker
    //  need to be identified
    if (1 == pHmpConfObj->m_clientMap.size() -1 ){
       continue;
    }
    for (auto itr = pHmpConfObj->m_clientMap.begin ();
                itr != pHmpConfObj->m_clientMap.end (); ++itr){

       dst_client = itr->second;
       if (dst_client && (dst_client->m_detail.m_callId != DUMMY_CALLID)
               && (dst_client->m_detail.m_joinerType != EN_JOINER_TYPE_SCREENSHARE)){
          dst_client->m_sampleInfo_mutex.lock();
          get_average_variation_samples (dst_client, &avg_rms, &avg_peak_variation, &avg_decay_variation);
          dst_client->m_sampleInfo_mutex.unlock();

          sSampleInfo.rms_dB = avg_rms;
          sSampleInfo.peak_dB = avg_peak_variation;
          sSampleInfo.decay_dB = avg_decay_variation;
          sSampleInfo.callId_str = dst_client->m_detail.m_callId.c_str();

          sSortSampleInfo.emplace_back(sSampleInfo);
          XGLOG_TRACE ("client: %s avg_rms: %f avg_peak_variation: %f avg_decay_variation: %f",
                     sSampleInfo.callId_str, sSampleInfo.rms_dB , sSampleInfo.peak_dB, sSampleInfo.decay_dB);
          new_active_spkr_callId = dst_client->m_detail.m_callId;
       }
    }

    // sort the collected samples by highest rms value
    std::sort (sSortSampleInfo.begin(), sSortSampleInfo.end(),
                        [](const AUDIO_SAMPLE_INFO& s1, const AUDIO_SAMPLE_INFO& s2)
                          {
                             if (!s1.callId_str)
                               return false;
                             else if (!s2.callId_str)
                               return true;
                             else if (s1.callId_str && s2.callId_str)
                               return s1.rms_dB >= s2.rms_dB;
                           });

    // Vector is sorted in decreasing order . so the first element will have highest
    // average RMS. However , we need to check the rms and peak variation between the values
    // so that we do not wrongly decide the active speaker
    if (sSortSampleInfo.size() > 1){
   #ifdef LOG_ACTIVE_SPK_INFO
       XGLOG_INFO ("==== SORTED order by rms_dB ===== ");
   #endif
       for (guint uiIndex = 0; uiIndex < sSortSampleInfo.size (); uiIndex++){
   #ifdef LOG_ACTIVE_SPK_INFO
         XGLOG_INFO ("sorted ---> client: %s avg_rms: %f avg_peak_variation: %f avg_decay_variation: %f",
          sSortSampleInfo[uiIndex].callId_str, sSortSampleInfo[uiIndex].rms_dB ,
          sSortSampleInfo[uiIndex].peak_dB, sSortSampleInfo[uiIndex].decay_dB);
   #endif
          if (0 == strcmp (sSortSampleInfo[uiIndex].callId_str, pHmpConfObj->m_activeSpeakerId.c_str())){
            current_actspkr_index = uiIndex;
          }
      }
      if (0 != strcmp (sSortSampleInfo[0].callId_str, pHmpConfObj->m_activeSpeakerId.c_str())){
        rms_variation = ((std::abs(sSortSampleInfo[0].rms_dB)
                          - std::abs(sSortSampleInfo[current_actspkr_index].rms_dB))
                       / std::abs(sSortSampleInfo[current_actspkr_index].rms_dB )) * 100;

        rms_variation = std::abs(rms_variation);

        peak_variation = ((std::abs((sSortSampleInfo[0].peak_dB)
                          - std::abs(sSortSampleInfo[current_actspkr_index].peak_dB)))
                       / std::abs(sSortSampleInfo[current_actspkr_index].peak_dB )) * 100;
   #ifdef LOG_ACTIVE_SPK_INFO
        XGLOG_INFO ("rms_variation : %f between client_0: %s client_curr_act_spkr: %s ",
                     rms_variation, sSortSampleInfo[0].callId_str, sSortSampleInfo[current_actspkr_index].callId_str);
   #endif
        peak_variation =  std::abs (peak_variation);

        if (rms_variation > RMS_PERCENT_VARIATION )
           new_active_spkr_callId = sSortSampleInfo[0].callId_str;
        else
           new_active_spkr_callId = sSortSampleInfo[current_actspkr_index].callId_str;
      } else{
          new_active_spkr_callId = pHmpConfObj->m_activeSpeakerId;
      }
    } else {
      new_active_spkr_callId = "";
    }

    // post newly detect active speaker message to SFU controller
    if (new_active_spkr_callId != pHmpConfObj->m_activeSpeakerId)
    {
       XGLOG_INFO ("current activ spkr: %s  new active spkrId: %s ",
                   pHmpConfObj->m_activeSpeakerId.c_str(), new_active_spkr_callId.c_str());

       pHmpConfObj->send_active_speaker_info (new_active_spkr_callId, FALSE);
    }

    sSortSampleInfo.clear();

  } // end of while loop
}

void xGateHmpConference::get_average_variation_samples(Client  *client,
                                                       gdouble  *avg_rms,
                                                       gdouble  *avg_peak_variation,
                                                       gdouble  *avg_decay_variation)
{
   guint uiIndex = 0;
   gdouble rms = 0;
   gdouble peak_variation = 0;
   gdouble decay_variation = 0;

   // if the last sample index is 0, it means we have only one sample data to analyze
   if (!client->m_last_index) {
     *avg_peak_variation = client->m_sample_info[uiIndex].peak_dB;
     *avg_decay_variation = client->m_sample_info[uiIndex].decay_dB;
     *avg_rms = client->m_sample_info[uiIndex].rms_dB;
   }else{
     for (uiIndex = 0; uiIndex <= client->m_last_index; //MAX_NUM_SAMPLES;
                      uiIndex++){
       if (uiIndex  < client->m_last_index){
         peak_variation += ((client->m_sample_info[uiIndex].peak_dB - client->m_sample_info[uiIndex + 1].peak_dB)
                       / client->m_sample_info[uiIndex + 1].peak_dB ) * 100;
         decay_variation += ((client->m_sample_info[uiIndex].decay_dB- client->m_sample_info[uiIndex + 1].decay_dB)
                       / client->m_sample_info[uiIndex + 1].decay_dB ) * 100;
        }
        rms += client->m_sample_info[uiIndex].rms_dB;
      }
      *avg_peak_variation = peak_variation / client->m_last_index;
      *avg_decay_variation = decay_variation / client->m_last_index ;
      *avg_rms = rms / (client->m_last_index + 1);
   }
   //reset the values to silence DB
   for (uiIndex = 0; uiIndex <= client->m_last_index; uiIndex++){
     client->m_sample_info[uiIndex].peak_dB = SILENCE_SAMPLE_VALUE_DB;
     client->m_sample_info[uiIndex].decay_dB = SILENCE_SAMPLE_VALUE_DB;
     client->m_sample_info[uiIndex].rms_dB = SILENCE_SAMPLE_VALUE_DB;
   }
   client->m_last_index = 0;
}

xGateRetVal xGateHmpConference::send_active_speaker_info(string &active_spkr_callId, gboolean bForceSendInfo)
{
  xGateHmpConference *pHmpConference = NULL;
  MgMediaDetail mediaDetail;
  gboolean      bSendMsg = FALSE;
  Client *client = NULL;
  Client *dst_client = NULL;

  pHmpConference = this;
  if(!pHmpConference){
    XGLOG_ERROR("send_active_speaker_info() failed as no conference object failed");
    return EN_XGATE_STATUS_ERROR;
  }
  client = pHmpConference->get_participant_entry(active_spkr_callId);
  if(!client){
    XGLOG_WARN("send_active_speaker_info() failed Client not found  for call_id: %s fetching last entry",
		                    active_spkr_callId.c_str());
    pHmpConference->fetch_last_connected_entry (pHmpConference);
    goto send_key_frame_req;
  }

  if ((pHmpConference->m_activeSpeakerId != active_spkr_callId) || (TRUE == bForceSendInfo))
    bSendMsg = TRUE;

  if (pHmpConference->m_Prev_activeSpeakerId.empty()){
    pHmpConference->m_Prev_activeSpeakerId = active_spkr_callId;
  }
  if(pHmpConference->m_activeSpeakerId.empty()){
    pHmpConference->m_activeSpeakerId = active_spkr_callId;
  }else{
    if ( pHmpConference->m_Prev_activeSpeakerId.empty() ||
       (pHmpConference->m_Prev_activeSpeakerId != pHmpConference->m_activeSpeakerId
          && active_spkr_callId != pHmpConference->m_activeSpeakerId))
    {
       pHmpConference->m_Prev_activeSpeakerId = pHmpConference->m_activeSpeakerId;
       XGLOG_INFO("xGateHmpConference::PreviousActive Speaker Id: %s",
                                            pHmpConference->m_Prev_activeSpeakerId.c_str());
       bSendMsg = TRUE;
    }
    if(pHmpConference->m_activeSpeakerId == active_spkr_callId){
      XGLOG_INFO("xGateHmpConference::Active Speaker Id received same as previous active speaker:%s",
                                            pHmpConference->m_activeSpeakerId.c_str());
    }else{
      pHmpConference->m_activeSpeakerId = active_spkr_callId;
    }
  }
  // check if current and previous active speaker are same. In that case, assign some other
  // callId as previous active speaker
  if (pHmpConference->m_Prev_activeSpeakerId == pHmpConference->m_activeSpeakerId){
    XGLOG_INFO("Active Speaker : %s == prev_active spkr: %s",
            pHmpConference->m_activeSpeakerId.c_str(), pHmpConference->m_Prev_activeSpeakerId.c_str());
    for (auto itr = pHmpConference->m_clientMap.begin ();
                itr != pHmpConference->m_clientMap.end (); itr++){
      dst_client = itr->second;
      if (dst_client && (0 != strcmp(dst_client->m_detail.m_callId.c_str(), DUMMY_CALLID))
         && (dst_client->m_detail.m_callId != pHmpConference->m_Prev_activeSpeakerId)){
         pHmpConference->m_Prev_activeSpeakerId.erase ();
         pHmpConference->m_Prev_activeSpeakerId = dst_client->m_detail.m_callId;
         XGLOG_INFO ("Assigning dst_client->m_detail.m_callId: %s act Prev ActiveSpkr",
                          pHmpConference->m_Prev_activeSpeakerId.c_str());
         bSendMsg = TRUE;
         break;
      }
    }
  }

send_key_frame_req:
  XGLOG_INFO("xGateHmpConference::Active Speaker Info received with Call_Id: %s prev_active spkr: %s",
            pHmpConference->m_activeSpeakerId.c_str(), pHmpConference->m_Prev_activeSpeakerId.c_str());
   for (auto itr = pHmpConference->m_clientMap.begin ();
                itr != pHmpConference->m_clientMap.end (); ++itr){
     dst_client = itr->second;
     if (dst_client && (0 != strcmp(dst_client->m_detail.m_callId.c_str(), DUMMY_CALLID))
        && dst_client->m_mid3CallType != XGATE_MID3_CALL_TYPE_SCREENSHARE){
       // copy the call_id of the screen share client in all other client's object
        if (dst_client->m_detail.m_callId != pHmpConference->m_activeSpeakerId){
          dst_client->m_mid3CallId.erase();
          dst_client->m_mid3CallId = pHmpConference->m_activeSpeakerId;
          XGLOG_INFO("xGateHmpConference:: client: %s mid3CallId: %s", dst_client->m_detail.m_callId.c_str(),
                            dst_client->m_mid3CallId.c_str());
        }
        else if (dst_client->m_detail.m_callId != pHmpConference->m_Prev_activeSpeakerId){
          dst_client->m_mid3CallId.erase();
          dst_client->m_mid3CallId = pHmpConference->m_Prev_activeSpeakerId;
          XGLOG_INFO("xGateHmpConference:: client: %s mid3CallId: %s", dst_client->m_detail.m_callId.c_str(),
                            dst_client->m_mid3CallId.c_str());
        }
        dst_client->m_mid3CallType = XGATE_MID3_CALL_TYPE_ACTIVE_SPEAKER;
      }
   }

  if(client && (bSendMsg== TRUE) && (client->m_mid3CallType != XGATE_MID3_CALL_TYPE_SCREENSHARE)) {
    MgMediaDetail mediaDetail;
    SFUController *pSFUController = xGateSFUtil::getSFUController();
    mediaDetail.mgresource_id = pHmpConference->m_conferenceId;
    mediaDetail.joiner_type = client->m_detail.m_joinerType;
    mediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
    mediaDetail.call_id = client->m_detail.m_callId;
    mediaDetail.prev_act_spkr = pHmpConference->m_Prev_activeSpeakerId;
    if (pSFUController){
      XGLOG_INFO( "Send Keyframe request new active speaker:%s",client->m_detail.m_callId.c_str());
      pSFUController->send_msg_sfu_client(&mediaDetail, client->m_detail,
                               EN_XGATE_MG_MEDIA_REQUEST_KEY_FRAME);

      XGLOG_INFO( "Sending Active speaker info: %s Prev active info: %s to PBX ",
                        mediaDetail.call_id.c_str(), mediaDetail.prev_act_spkr.c_str());
      pSFUController->send_msg_sfu_client(&mediaDetail, client->m_detail,
                               EN_XGATE_MG_ACTIVE_SPEAKER_INFO);
    }
  }
  return EN_XGATE_STATUS_SUCCESS;
}

void xGateHmpConference::fetch_last_connected_entry (void *pVoidObj)
{
   string entry="";
   Client *client = nullptr;
   gint  iIndex = 0;
   xGateHmpConference *pxGateHmpConfObj = (xGateHmpConference *) pVoidObj;

   for(iIndex = m_clientVector.size() - 1; iIndex >= 0 ; iIndex--) {
     client = m_clientVector[iIndex];
     if(client ){
       XGLOG_INFO("connected client at index: %d callId: %s", iIndex, client->m_detail.m_callId.c_str());
     }
     if (client && ((client->m_detail.m_joinerType == EN_JOINER_TYPE_SCREENSHARE)
          || (0 == strcmp (client->m_detail.m_callId.c_str(), DUMMY_CALLID)))){
       continue;
     }
     else if (client && pxGateHmpConfObj->m_activeSpeakerId.empty()){
         pxGateHmpConfObj->m_activeSpeakerId = client->m_detail.m_callId;
     }
     else if (client && (!pxGateHmpConfObj->m_activeSpeakerId.empty())
                     && pxGateHmpConfObj->m_activeSpeakerId != client->m_detail.m_callId){
         pxGateHmpConfObj->m_Prev_activeSpeakerId.clear();
         pxGateHmpConfObj->m_Prev_activeSpeakerId = client->m_detail.m_callId;
         break;
     }
     client = nullptr;
   }
   XGLOG_INFO ("m_activeSpeakerId: %s  m_Prev_activeSpeakerId: %s ",
               pxGateHmpConfObj->m_activeSpeakerId.c_str(),
               pxGateHmpConfObj->m_Prev_activeSpeakerId.c_str());
}

void xGateHmpConference::find_initial_active_spkr_client ()
{
  xGateHmpConference *pxGateHmpConfObj = (xGateHmpConference *) this;
  Client   *client = nullptr;
  gdouble avg_rms = 0;
  gdouble avg_peak_variation = 0;
  gdouble  avg_decay_variation = 0;
  guint     uiNumClientMuted = 0;
  guint     uiNumClient = 0;

  for(auto itr = pxGateHmpConfObj->m_clientMap.begin();
           itr != pxGateHmpConfObj->m_clientMap.end(); ++itr){
     client = itr->second;
     if ((!client) || (client->m_detail.m_joinerType == EN_JOINER_TYPE_SCREENSHARE)
          || (0 == strcmp (client->m_detail.m_callId.c_str(), DUMMY_CALLID))){
       continue;
     }
     client->m_sampleInfo_mutex.lock();
     get_average_variation_samples (client, &avg_rms, &avg_peak_variation, &avg_decay_variation);
     XGLOG_INFO ("client: %s avg_rms: %f avg_peak_variation: %f avg_decay_variation: %f",
                     client->m_detail.m_callId.c_str(),
                     avg_rms, avg_peak_variation, avg_decay_variation);

     if ((avg_rms == SILENCE_SAMPLE_VALUE_DB) && (avg_peak_variation == SILENCE_SAMPLE_VALUE_DB)
                   && (avg_decay_variation == SILENCE_SAMPLE_VALUE_DB)) {
       uiNumClientMuted++;
     }
     uiNumClient++;
     client->m_sampleInfo_mutex.unlock();
  }
  XGLOG_INFO ("Num of clients audio muted: %d  Num of clients in conference: %d",
                      uiNumClientMuted , uiNumClient);

  if (uiNumClientMuted == uiNumClient){
    XGLOG_INFO ("All participants muted at time of joining... modifying current and prev activ spkr");
    pxGateHmpConfObj->m_activeSpeakerId.clear();
    pxGateHmpConfObj->fetch_last_connected_entry (pxGateHmpConfObj);
    pxGateHmpConfObj->send_active_speaker_info (pxGateHmpConfObj->m_activeSpeakerId, TRUE);
  }
}

gboolean HmpFrameBin::Detect_voice_activity (Client *client,void *vBuffObj)
{
  GstBufferList   *list = NULL;
  GstBuffer       *buffer = NULL;
  guint           num_buffers = 0;
  HmpFrameBin     *frameBin = this;
  guint8          u8Level = 0xFF;
  // first check if audio level is signalled . if not signalled, we have
  // to decode it always
  if (frameBin->m_negotiated_audio_level_Id <= 0){
    return TRUE;
  }

  if (GST_IS_BUFFER_LIST (vBuffObj))
    list = GST_BUFFER_LIST_CAST (vBuffObj);
  else
    buffer = GST_BUFFER_CAST (vBuffObj);

  if (list){
    num_buffers = gst_buffer_list_length (list);
  }else if (buffer){
    num_buffers = 1;
  }

  for (guint uiIndex = 0; uiIndex < num_buffers; uiIndex++){
    if (list)
      buffer = gst_buffer_list_get (list, uiIndex);
    if (buffer){
      GstRTPBuffer rtp = { NULL };
      if ( TRUE != gst_rtp_buffer_map (buffer, GST_MAP_READWRITE, &rtp))
        return TRUE;
      gpointer data = 0;
      guint  size = 0;
      if (TRUE != gst_rtp_buffer_get_extension_onebyte_header (&rtp,
                            m_negotiated_audio_level_Id, 0, &data, &size) ){
        XGLOG_TRACE ("Failed to get extension data for callId: %s ssrc: %u extn: %d data: %0x size: %d",
                        client->m_detail.m_callId.c_str(),
                        gst_rtp_buffer_get_ssrc (&rtp),
                        m_negotiated_audio_level_Id, data, size);
        gst_rtp_buffer_unmap (&rtp);
        return TRUE;
      }else {
        // As per rfc6464, audio level is encoded in 7 bits as -dBov
        // Value range from 0 to 127 with 127 encoded as silence
        u8Level = *(guint8*)data & 0x7F;
        XGLOG_TRACE ("audio level for client: %s ssrc: %u is %u ",
                     client->m_detail.m_callId.c_str(), gst_rtp_buffer_get_ssrc (&rtp), u8Level );
        gst_rtp_buffer_unmap (&rtp);
        if (u8Level >= CUTOFF_AUDIO_LEVEL_TO_DECODE){
          return FALSE;
        }
        return TRUE;
      }
    }
  }
  return TRUE;
}
