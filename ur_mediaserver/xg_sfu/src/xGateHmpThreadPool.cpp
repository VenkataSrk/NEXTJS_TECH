#ifndef _XGATE_HMP_THREAD_POOL_H
#define _XGATE_HMP_THREAD_POOL_H

//gstreamer include
extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
}
//Added for thread lock mechanism
#include <mutex>
#include <condition_variable>
//local include
#include "xGateUtil.h"
#include "xGateSFUtil.h"
#include "xGateMgMsg.h"
#include "xGateHmpGstUtil.h"
#include "xGateHmpThreadPool.h"
#include "SFUController.h"
#include <sys/time.h>
#include <time.h>

xGateRetVal ThreadPool::create_thread_pool (void * vPtr,
                                           XGATE_HMP_THREAD_POOL_TYPE ePoolType,
                                           bool bExclusive)
{
  GError   **error = nullptr;
  GFunc    funcPtr;
  GThreadPool  *threadPool = nullptr;
  gint      max_threads = -1;

  if (THREAD_POOL_EVENT == ePoolType){
    funcPtr = event_thread_pool_handler;
    m_event_pool = NULL;
    if (TRUE == bExclusive)
      max_threads = MAX_EVENT_HANDLING_THREADS;

  } else if (THREAD_POOL_RX_DATA == ePoolType) {
    funcPtr = rcv_data_thread_pool_handler;
    m_rx_data_pool = NULL;
    if (TRUE == bExclusive)
      max_threads = MAX_RX_DATA_RCV_THREADS;

  } else if (THREAD_POOL_AUDIO_DEC == ePoolType) {
    m_decoder_pool = NULL;
    funcPtr = audio_dec_thread_pool_handler;
    if (TRUE == bExclusive)
      max_threads = MAX_AUDIO_DEC_THREADS;

  } else if (THREAD_POOL_AUDIO_ENC == ePoolType) {
    funcPtr = audio_enc_thread_pool_handler;
    m_encoder_pool = NULL;
    if (TRUE == bExclusive)
      max_threads = MAX_AUDIO_ENC_THREADS;

  } else {
    XGLOG_ERROR ("Unknow PoolType: %d returning error", ePoolType);
    return EN_XGATE_STATUS_ERROR;
  }


  threadPool = g_thread_pool_new (funcPtr,
                                    vPtr,
                                    max_threads,
                                    bExclusive, // exclusive is false
                                    error);
  if (nullptr == threadPool){
    XGLOG_INFO ("Failed to create event thread pool error: %s", error);
    return EN_XGATE_STATUS_ERROR;
  }

  if (THREAD_POOL_EVENT == ePoolType){
    m_event_pool = threadPool;
  } else if (THREAD_POOL_RX_DATA == ePoolType) {
    m_rx_data_pool = threadPool;
  } else if (THREAD_POOL_AUDIO_DEC == ePoolType) {
    m_decoder_pool = threadPool;
  } else if (THREAD_POOL_AUDIO_ENC == ePoolType) {
    m_encoder_pool = threadPool;
  }

  return EN_XGATE_STATUS_SUCCESS;
}

void ThreadPool::event_thread_pool_handler (gpointer data, gpointer gpData)
{
  xGateHmpClientThreadMsg *pMsg = (xGateHmpClientThreadMsg *)data;
  Client                  *client = nullptr;
  SFUController           *pSFUController = xGateSFUtil::getSFUController();

  if (!pMsg)
    return;

   client = (Client *) pMsg->pClient;

   if (!client || client->m_detail.m_callId.empty()){
     XGLOG_ERROR ("event_thread_pool_handler () client ptr is null or callId field is empty");
     delete pMsg;
     return;
   }
   XGLOG_TRACE ("Entered event_thread_pool_handler() with client: %s threadPool: %p",
                     client->m_detail.m_callId.c_str(),
                     pSFUController->m_ThreadPool_info.m_event_pool);
   client->process_client_msg (pMsg);
   // delete the message received
   if(pMsg)
   {
     delete pMsg;
     pMsg = NULL;
   }

   XGLOG_TRACE ("EXITING event_thread_pool_handler() with client: %s ",
                     client->m_detail.m_callId.c_str());
}


void ThreadPool::audio_dec_thread_pool_handler(gpointer data, gpointer gpData)
{

  xGateHmpClientThreadMsg *pMsg = (xGateHmpClientThreadMsg *)data;
  Client                  *client = nullptr;
  SFUController           *pSFUController = xGateSFUtil::getSFUController();
  HmpAudioDecoderBin      *Decoderbin = NULL;
  GstBufferList           *list = NULL;
  GstBuffer               *buffer = NULL;
  void                    *param = NULL;

  if (!pMsg)
    return;

   client = (Client *) pMsg->pClient;

   if (!client || client->m_detail.m_callId.empty()){
     XGLOG_ERROR ("audio_dec_thread_pool_handler () client ptr is null or callId field is empty");
     delete pMsg;
     return;
   }

   param = pMsg->param2;
   if (GST_IS_BUFFER_LIST (param))
     list = GST_BUFFER_LIST_CAST (param);
   else
     buffer = GST_BUFFER_CAST (param);


   if (TRUE == client->m_StopSendRecvData){
     XGLOG_TRACE ("client is in Stopping state. unreffing buffer and returning");
     if (list){
       gst_buffer_list_unref (list);
     }
     else{
       gst_buffer_unref (buffer);
     }

     delete pMsg;
     return;
   }

   g_mutex_lock (&client->m_audio_decode_process_lock);

   XGLOG_TRACE ("Entered audio_dec_thread_pool_handler () client: %s threadId: %d MsgNum: %llu buffer: %p name: %s ",
                     client->m_detail.m_callId.c_str(),
                     syscall (SYS_gettid), pMsg->ullMsgNumber, pMsg->param2, pMsg->ucName);
   if (client->m_FrameChannel){
     Decoderbin = client->m_FrameChannel->m_pHmpAudioDecoderBin;
     if ((pMsg->ullMsgNumber - client->ullLastAudioDecodedMsgNum) > 1){
       XGLOG_INFO ("client: %s Pending the Audio Decoding of Msg: %llu as diff > 1 LastDecoded: %llu",
                        client->m_detail.m_callId.c_str(), pMsg->ullMsgNumber,
                        client->ullLastAudioDecodedMsgNum);
       g_mutex_unlock (&client->m_audio_decode_process_lock);
       g_thread_pool_push (pSFUController->m_ThreadPool_info.m_decoder_pool, pMsg, NULL);
       return;
     }
   }
   client->ullLastAudioDecodedMsgNum = pMsg->ullMsgNumber;

   if(Decoderbin){
     GstPad *sinkPad = Decoderbin->m_ghostRtpSinkPad;
     if (list)
        gst_pad_chain_list_without_lock (sinkPad, list);
     else
        gst_pad_chain_without_lock (sinkPad, buffer);
   }
   g_mutex_unlock (&client->m_audio_decode_process_lock);

   XGLOG_TRACE ("EXIT audio_dec_thread_pool_handler () client: %s threadId: %d MsgNum: %llu buffer: %p name: %s ",
                     client->m_detail.m_callId.c_str(),
                     syscall (SYS_gettid), pMsg->ullMsgNumber, pMsg->param2, pMsg->ucName);
   delete pMsg;

}

void ThreadPool::audio_enc_thread_pool_handler(gpointer data, gpointer gpData)
{
  xGateHmpClientThreadMsg *pMsg = (xGateHmpClientThreadMsg *)data;
  Client                  *client = nullptr;
  GstBuffer               *buffer = NULL;
  SFUController           *pSFUController = xGateSFUtil::getSFUController();
  HmpAudioEncoderBin      *pAudioBin = NULL;
  GstPad                  *sinkPad = NULL;
  struct timeval          tv1 = {0,0};
  struct timeval          tv2 = {0,0};
  if (!pMsg)
    return;

   client = (Client *) pMsg->pClient;

   if (!client || client->m_detail.m_callId.empty()){
     XGLOG_ERROR ("audio_enc_thread_pool_handler () client ptr is null or callId field is empty");
     delete pMsg;
     return;
   }

   buffer = pMsg->param2;

   if (TRUE == client->m_StopSendRecvData){
     XGLOG_TRACE ("client is in Stopping state. unreffing buffer and returning");
     gst_buffer_unref (buffer);
     delete pMsg;
     return;
   }
   XGLOG_TRACE ("audio_enc_thread_pool_handler () client: %s threadId: %d MsgNum: %llu buffer: %p dts: %lld pts: %lld",
                     client->m_detail.m_callId.c_str(),
                     syscall (SYS_gettid), pMsg->ullMsgNumber, buffer,
                     GST_BUFFER_DTS (buffer), GST_BUFFER_PTS (buffer));

   g_mutex_lock (&client->m_audio_encode_process_lock);

   if (client->m_FrameChannel)
     pAudioBin  = client->m_FrameChannel->m_pHmpAudioEncoderBin;

   if ((pMsg->ullMsgNumber - client->ullLastAudioEncodeMsgNum) > 1) {
     XGLOG_INFO ("client: %s Pending the Audio Encoding of Msg: %llu as diff > 1 LastEncoded: %llu",
                        client->m_detail.m_callId.c_str(), pMsg->ullMsgNumber,
                        client->ullLastAudioEncodeMsgNum);
     g_mutex_unlock (&client->m_audio_encode_process_lock);
     g_thread_pool_push (pSFUController->m_ThreadPool_info.m_encoder_pool, pMsg, NULL);
     return;
   }
   client->ullLastAudioEncodeMsgNum = pMsg->ullMsgNumber;

   g_mutex_unlock (&client->m_audio_encode_process_lock);

   gettimeofday (&tv1, NULL);
   sinkPad = gst_element_get_static_pad (pAudioBin->m_opusenc_filter, "sink");
   if (sinkPad)
     gst_pad_chain (sinkPad, buffer);

   gettimeofday (&tv2, NULL);

  // Unref the buffer that was ref in GstAduioMixerCallback function
   gst_buffer_unref (buffer);
   guint64 diff_ms = (tv2.tv_sec - tv1.tv_sec ) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000;
   if (diff_ms > 4) {
     XGLOG_INFO ("client: %s tv2.tv_sec: %ld tv1.tv_sec: %ld tv2.tv_usec: %ld tv1.tv_usec: %ld ",
                 client->m_detail.m_callId.c_str(),
                 tv2.tv_sec, tv1.tv_sec,
                 tv2.tv_usec, tv1.tv_usec);
     XGLOG_INFO ("client: %s Thread: %d Time to encode buffer: %p dts: %lld pts: %lld is diff_ms: %llu > 4 ms",
           client->m_detail.m_callId.c_str(), syscall (SYS_gettid), buffer,
           GST_BUFFER_DTS (buffer), GST_BUFFER_PTS (buffer), diff_ms);
   }
   delete pMsg;
   XGLOG_TRACE ("EXIT from audio_enc_thread_pool_handler () with client: %s threadId: %d buffer: %p",
                     client->m_detail.m_callId.c_str(), syscall (SYS_gettid), buffer);
}

void ThreadPool::rcv_data_thread_pool_handler(gpointer data, gpointer gpData)
{
  xGateHmpClientThreadMsg *pMsg = (xGateHmpClientThreadMsg *)data;
  Client                  *client = nullptr;
  SFUController           *pSFUController = xGateSFUtil::getSFUController();
  HmpFrameBin             *frameBin = NULL;
  void                    *pvObj = NULL;
   if (!pMsg)
     return;

   client = (Client *) pMsg->pClient;
   pvObj = pMsg->param2;

   if (!client || client->m_detail.m_callId.empty()){
     XGLOG_ERROR ("rcv_data_thread_pool_handler () client ptr: %p is null or callId field is empty",
             client);
     goto free_buffer;
   }


   if (TRUE == client->m_StopSendRecvData){
     XGLOG_INFO ("client is in Stopping state. unreffing buffer and returning");
     goto free_buffer;
   }
   if (client->m_FrameChannel)
     frameBin = client->m_FrameChannel->m_pHmpFrameBin;
   if (frameBin){
     g_mutex_lock (&client->m_data_process_lock);
     XGLOG_TRACE ("Entered rcv_data_thread_pool_handler () client: %s "
                     "threadId: %d MsgNum: %llu buffer_list: %p name: %s ",
                     client->m_detail.m_callId.c_str(),
                     syscall (SYS_gettid), pMsg->ullMsgNumber, pMsg->param2, pMsg->ucName);
     switch (pMsg->m_msgType)
     {
       case XGATE_CLIENT_RCVD_RTP_DATA:
         if (strstr (pMsg->ucName, "Audio" )){
            XGLOG_TRACE ("client: %s pMsg->ullMsgNumberAudio: %llu ullLastProcessedMsg: %llu ",
                   client->m_detail.m_callId.c_str(), pMsg->ullMsgNumber,
                   client->ullLastProcessedAudioMsgNum);
          if ((pMsg->ullMsgNumber - client->ullLastProcessedAudioMsgNum) > 1){
            XGLOG_INFO ("client: %s Pending process Audio as diff of Msg: %llu and last processed: %llu > 1",
                        client->m_detail.m_callId.c_str(),
                        pMsg->ullMsgNumber, client->ullLastProcessedAudioMsgNum);
            g_mutex_unlock (&client->m_data_process_lock);
            g_thread_pool_push (pSFUController->m_ThreadPool_info.m_rx_data_pool, pMsg, NULL);
            return;
          } else{
            client->ullLastProcessedAudioMsgNum = pMsg->ullMsgNumber;
          }
        } else if (strstr (pMsg->ucName, "Video" )){
           XGLOG_TRACE ("client: %s pMsg->ullMsgNumberVideo: %llu ullLastProcessedMsg: %llu",
                   client->m_detail.m_callId.c_str(), pMsg->ullMsgNumber,
                   client->ullLastProcessedVideoMsgNum);
           if ((pMsg->ullMsgNumber - client->ullLastProcessedVideoMsgNum) > 1){
             XGLOG_INFO ("client: %s Pending process Video as diff of Msg: %llu and last processed: %llu > 1",
                        client->m_detail.m_callId.c_str(), pMsg->ullMsgNumber,
                        client->ullLastProcessedVideoMsgNum);
             g_mutex_unlock (&client->m_data_process_lock);
             g_thread_pool_push (pSFUController->m_ThreadPool_info.m_rx_data_pool, pMsg, NULL);
             return;
           } else{
             client->ullLastProcessedVideoMsgNum = pMsg->ullMsgNumber;
           }
        }

       XGLOG_TRACE ("EXITING rcv_data_thread_pool_handler () with client: %s threadPool_threadId: %d buffer_list: %p name: %s",
                     client->m_detail.m_callId.c_str(),
                     syscall (SYS_gettid), pMsg->param2, pMsg->ucName);
       // Unlock after data is processed , otherwise the order of processing
       // the data is not guaranteed as thread scheduling is not in our
       // control
       frameBin->on_new_data_rcvd_srcPad(client, pMsg->ucName, pMsg->param2);
       break;

       case XGATE_CLIENT_REC_EVENT_REQUEST:
       {
         Client *client = static_cast <Client *> (pMsg->param1);
         xGateMediaEvent recEvent = *(xGateMediaEvent *)pMsg->param2;
         HmpRecordSendChannel *channel = static_cast <HmpRecordSendChannel *> (pMsg->param3);
         XGLOG_INFO ("received XGATE_CLIENT_REC_EVENT_REQUEST for client: %s record event: %d",
                     client->m_detail.m_callId.c_str(), recEvent);
         if (recEvent == EN_MEDIA_EVENT_RECORD_STOP){
            if (channel){
              XGLOG_INFO ("stopping the recording channel for call_id: %s", client->m_detail.m_callId.c_str());
              delete channel;
              channel = nullptr;
            }
         }
         if (pMsg->param2)
           delete pMsg->param2;
       }
       break;

       case XGATE_CLIENT_TILE_CHANGE_REQUEST:
       {
         Client *client = static_cast <Client *> (pMsg->param2);
         PARTICIPANT_LIST *client_list = static_cast <PARTICIPANT_LIST *>(pMsg->param1);
         SFUController *pSFUController = xGateSFUtil::getSFUController();
         if(!client_list){
           XGLOG_ERROR("client list passed is NULL in TILE_CHANGE_REQUEST");
           break;
         }
         XGLOG_INFO ("Received XGATE_CLIENT_TILE_CHANGE_REQUEST from SFU controller");
         if(FALSE == client->handle_tile_change_request(client_list)){
           XGLOG_INFO("Failed to handle Tile Change Request with callID: %s",
                          client->m_detail.m_callId.c_str());
         }
         if(client_list->size()){
          client_list->clear();
         }
         delete client_list;
       }
       break;

       default:
       break;
     }
     g_mutex_unlock (&client->m_data_process_lock);
   }

   delete pMsg;

   return;

free_buffer:
   delete pMsg;
   if (GST_IS_BUFFER_LIST (pvObj))
     gst_buffer_list_unref (GST_BUFFER_LIST_CAST (pvObj));
   else
     gst_buffer_unref (GST_BUFFER_CAST (pvObj));
}


#endif //_XGATE_HMP_THREAD_POOL_H
