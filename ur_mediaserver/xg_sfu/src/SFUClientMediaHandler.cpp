#include <gst/app/gstappsrc.h>
#include "xGateUtil.h"
#include "xGateSFUtil.h"
#include "SFUClient.h"
#include "SFUClientMediaHandler.h"
#include "xglog.h"

#define THISMODULE "SFU_MEDIA_HANDLER"

#ifdef RECORD_ENABLED
/* SFUClientMediaHandler class implmentation - start */
SFUClientMediaHandler::SFUClientMediaHandler(gpointer userdata, gint maxThreads, gboolean exclusive) :
  m_tpool(NULL), m_userdata(userdata), m_maxThreads(maxThreads), m_exclusive(exclusive)
{

}

SFUClientMediaHandler::~SFUClientMediaHandler()
{
  if(m_tpool) {
    g_thread_pool_free(m_tpool, TRUE, TRUE);
  }
}
/* SFUClientMediaHandler class implmentation - end */

/* SFUClientRcvdVideoHandler class implmentation - start */
SFUClientRcvdVideoHandler::SFUClientRcvdVideoHandler(gpointer userdata, gint maxThreads, gboolean exclusive) :
  SFUClientMediaHandler(userdata, maxThreads, exclusive)
{
  if(maxThreads > MAX_RCVD_VIDEO_HANDLER_THREADS) {
    maxThreads = MAX_RCVD_VIDEO_HANDLER_THREADS;
  }
}

SFUClientRcvdVideoHandler::~SFUClientRcvdVideoHandler()
{

}

gboolean SFUClientRcvdVideoHandler::init()
{
  if(m_tpool) {
    XGLOG_ERROR( "SFUClientRcvdVideoHandler::init failed. Thread pool already been initialized !");
    return FALSE;
  }

  GError **err;
  m_tpool = g_thread_pool_new((GFunc)rcvd_video_handler, m_userdata, m_maxThreads, m_exclusive, err);
  if(err && m_tpool == NULL) {
    XGLOG_ERROR("SFUClientRcvdVideoHandler::init failed. Error: %s occurred while creating thread pool !", err);
    return FALSE;
  }

  XGLOG_INFO("SFUClientRcvdVideoHandler::init success");

  return TRUE;
}

void SFUClientRcvdVideoHandler::rcvd_video_handler(gpointer data, gpointer userdata)
{
  Client *srcClient = (Client *)data;
  xGateHmpConference *pHmpConf = (xGateHmpConference *)userdata;

  if(srcClient == NULL) {
    XGLOG_ERROR("Handling received video buffer failed. Source client is NULL!");
    return;
  }

  string id = srcClient->m_detail.m_callId;
  //XGLOG_TRACE("Received a video buffer from a client '%s'", id.c_str());

  //dequeue buffer list from rcvd_video_queue
  GstBufferList *list = NULL;
  gpointer glist = NULL;
  GAsyncQueue *rcvd_video_queue = srcClient->m_rcvd_video_queue;
  if(rcvd_video_queue) {
    glist = g_async_queue_try_pop(rcvd_video_queue);
    if(glist) {
      list = (GstBufferList *)glist;
    }
  }

  if(list == NULL) {
    XGLOG_ERROR("Handling received video buffer list failed. Received video buffer list is not valid!");
    return;
  }

  //push received buffer list to recording server
  if(!push_to_record_video_handler(pHmpConf, srcClient, list)) {
     //XGLOG_ERROR("Pushing video buffer to recording server failed for client: %s", id.c_str());
  }
  //push recevied buffer list to source client thread
  if(!push_to_client_thread(srcClient, list)) {
    return;
  }
  return;
}

gboolean SFUClientRcvdVideoHandler::push_to_client_thread(Client *client, GstBufferList *list)
{
  HmpFrameBin *frameBin = NULL;
  ACE_Message_Block* pAmb = NULL;
  xGateHmpClientThreadMsg *pMsg = NULL;
  ACE_Time_Value tval ((time(NULL) ));
  
  frameBin = client->m_FrameChannel->m_pHmpFrameBin;

  /* Post message to thread if thread is created , otherwise post the
   * buffer to sender thread. Need to check if RCV thread is required 
   */
  if(client->rcv_data_thrId) {
    //XGLOG_DEBUG("Pushing video buffer to Client thread");
    pMsg = new xGateHmpClientThreadMsg ();
    tval += ACE_Time_Value(0,1);
    strcpy (pMsg->ucName, "Video");
    pMsg->m_msgType = XGATE_CLIENT_RCVD_RTP_DATA;
    pMsg->param2 = (void *) (list);
    pMsg->param1 = (void *) (client);
    
    pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
    if(client->rcv_data_msg_q) {
      if(!client->rcv_data_msg_q->enqueue_prio(pAmb, &tval)) {
        XGLOG_ERROR("Failed to push RTP data from padProbe callback to Client thread");
        return FALSE;
      }
    } else {
      XGLOG_ERROR("Failed to push RTP data from padProbe callback rcv_data_msg_q is NULL");
      return FALSE;
    }   
  } else {
    frameBin->on_new_data_rcvd_srcPad(client, "Video", list);
  }
  return TRUE;
}

gboolean SFUClientRcvdVideoHandler::push_to_record_video_handler(
                                      xGateHmpConference *hmpConf,
                                      Client *client,
                                      GstBufferList *list)
{
  SFUController *pHmpGstManager = NULL;
  pHmpGstManager = xGateSFUtil::getSFUController();
  GError *err = NULL;
	
  if(hmpConf->m_recorderMap.size() <=0 ) {
    return FALSE;
  }

  SFUClientRecordVideoHandler *pHandler = pHmpGstManager->get_record_video_handler();
  if(!pHandler) {
    XGLOG_ERROR("RecordVideoHandler is Null!");
    return FALSE;
  }

  GThreadPool *pHandlerPool = pHandler->get_record_video_handler_pool();
  if(!pHandlerPool) {
    XGLOG_ERROR("ReceivedVideoHandler pool is Null!");
    return FALSE;
  }

  //push buffer_list to record video handler queue
  GAsyncQueue *queue = hmpConf->m_recorder_video_queue;
  if(queue) {
    gint unprocessed = g_async_queue_length(queue);
    gst_buffer_list_ref (list);
    g_async_queue_push(queue, (gpointer)list);
  }

  g_thread_pool_push(pHandlerPool, client, &err);
  if(err) {
    XGLOG_ERROR("Pushing a record video buffer to RecorderVideoHandler thread failed. '%s'.", err->message);
    g_error_free(err);
    return FALSE;
  }

  return TRUE;
}

GThreadPool *SFUClientRcvdVideoHandler::get_rcvd_video_handler_pool()
{
  return m_tpool;
}
/* SFUClientRcvdVideoHandler class implmentation - end */

/* SFUClientRecordVideoHandler class implmentation - start */
SFUClientRecordVideoHandler::SFUClientRecordVideoHandler(
                                   gpointer userdata,
                                   gint maxThreads,
                                   gboolean exclusive) :
  SFUClientMediaHandler(userdata, maxThreads, exclusive)
{
  if(maxThreads > MAX_RECORD_VIDEO_HANDLER_THREADS) {
    maxThreads = MAX_RECORD_VIDEO_HANDLER_THREADS;
  }
}

SFUClientRecordVideoHandler::~SFUClientRecordVideoHandler()
{

}

gboolean SFUClientRecordVideoHandler::init()
{
  GError **err = NULL;
  if(m_tpool) {
    XGLOG_ERROR( "SFUClientRecordVideoHandler::init failed. Thread pool already been initialized !");
    return FALSE;
  }

  m_tpool = g_thread_pool_new((GFunc)record_video_handler, m_userdata, m_maxThreads, m_exclusive, err);
  if(err && m_tpool == NULL) {
    XGLOG_ERROR("SFUClientRecordVideoHandler::init failed. Error: %s ", err);
    return FALSE;
  }

  XGLOG_INFO("SFUClientRecordVideoHandler::init success");

  return TRUE;
}

void SFUClientRecordVideoHandler::record_video_handler(gpointer data, gpointer userdata)
{
  Client *srcClient = (Client *)data;
  xGateHmpConference *pHmpConf = (xGateHmpConference *)userdata;

  if(srcClient == NULL) {
    XGLOG_ERROR("Handling record video buffer failed. Source client is NULL!");
    return;
  }

  string id = srcClient->m_detail.m_callId;
  XGLOG_TRACE("Received a video buffer from a client '%s'", id.c_str());

  //dequeue buffer list from rcvd_video_queue
  GstBufferList *list = NULL;
  gpointer glist = NULL;
  GAsyncQueue *queue = pHmpConf->m_recorder_video_queue;
  if(queue) {
    glist = g_async_queue_try_pop(queue);
    if(glist) {
      list = (GstBufferList *)glist;
    }
  }

  if(list == NULL) {
    XGLOG_ERROR("Handling received video buffer list failed. Received video buffer list is not valid!");
    return;
  }

#if 0
  //push received buffer list to recording server
  if(!push_to_recorder(pHmpConf, srcClient, list)) {
    //XGLOG_ERROR("Pushing video buffer to recording server failed for client: %s", id.c_str());
  }
#endif
  return;
}

gboolean SFUClientRecordVideoHandler::send_to_recorder(HmpRecordSendChannel *channel, GstBufferList *list)
{
  HmpRecordVideoSendChannel *videoSendChannel = channel->m_videoSendChannel;
  if(videoSendChannel && videoSendChannel->m_start && videoSendChannel->m_pHmpRecVideoSendBin) {
    GstPad *sinkPad = videoSendChannel->m_pHmpRecVideoSendBin->m_udpSinkPad;
    if(sinkPad) {
      GstBufferList *rec_buffer = gst_buffer_list_copy_deep(list);
      //gst_pad_chain_list(sinkPad, rec_buffer);
      gst_pad_chain_list_without_lock (sinkPad, rec_buffer);
    }
  }
  return TRUE;
}

gboolean SFUClientRcvdVideoHandler::push_to_recorder(xGateHmpConference *hmpConf,
                                                    Client *client, GstBufferList *list)
{
  string activeSpeakerId = hmpConf->m_activeSpeakerId;
  string srcClientId = client->m_detail.m_callId;
  bool activeSpeaker = false;

  if(hmpConf->m_recorderMap.empty()) {
    return FALSE;
  }

  if(!activeSpeakerId.empty()) { //if there is no active speaker
    activeSpeaker = true;
  }

  RECORDER_MAP::iterator itr = hmpConf->m_recorderMap.begin();
  for(itr = hmpConf->m_recorderMap.begin(); itr != hmpConf->m_recorderMap.end(); itr++) {
    HmpRecordSendChannel *sendChannel = itr->second;
    if(sendChannel){
      HmpRecordVideoSendChannel *videoSendChannel = sendChannel->m_videoSendChannel;
      if(videoSendChannel && videoSendChannel->m_start
             && videoSendChannel->m_pHmpRecVideoSendBin) {
         GstPad *sinkPad = videoSendChannel->m_pHmpRecVideoSendBin->m_udpSinkPad;
         if(sinkPad) {
           GstBufferList *rec_buffer = gst_buffer_list_copy_deep(list);
          /*
           * gst_pad_chain_without_lock should not be called as this buffer is
           * accessed by other client thread and problems of hang are observed
           * when recording is enabled
           */
           gst_pad_chain_list (sinkPad, rec_buffer);
         }
       }
    }
  } //for loop end
  return TRUE;
}

gboolean SFUClientRecordVideoHandler::push_to_recorder(xGateHmpConference *hmpConf, Client *client, GstBufferList *list)
{
  string activeSpeakerId = hmpConf->m_activeSpeakerId;
  string srcClientId = client->m_detail.m_callId;
  bool activeSpeaker = false;
	
  if(hmpConf->m_recorderMap.empty()) {
    XGLOG_INFO("SFUClientRecordVideoHandler::push_to_recorder m_recorderMap is empty");
    gst_buffer_list_unref(list);
    return FALSE;
  }

  if(!activeSpeakerId.empty()) { //if there is no active speaker
    activeSpeaker = true;	
  }

  RECORDER_MAP::iterator itr = hmpConf->m_recorderMap.begin();
  for(itr = hmpConf->m_recorderMap.begin(); itr != hmpConf->m_recorderMap.end(); itr++) {
    HmpRecordSendChannel *sendChannel = itr->second;
    if(sendChannel){
      XGLOG_TRACE("srcClientId: %s m_recorderMap.size(): %d",
         srcClientId.c_str(), hmpConf->m_recorderMap.size());
      send_to_recorder(sendChannel, list);
    }
  } //for loop end
  gst_buffer_list_unref(list);
  return TRUE;
}

GThreadPool *SFUClientRecordVideoHandler::get_record_video_handler_pool()
{
  return m_tpool;
}
/* SFUClientRecordVideoHandler class implmentation - end */

/* SFUClientRcvdAudioHandler class implmentation - start */
SFUClientRcvdAudioHandler::SFUClientRcvdAudioHandler(gpointer userdata, gint maxThreads, gboolean exclusive) :
  SFUClientMediaHandler(userdata, maxThreads, exclusive)
{
  if(maxThreads > MAX_RCVD_AUDIO_HANDLER_THREADS) {
    maxThreads = MAX_RCVD_AUDIO_HANDLER_THREADS;
  }
}

SFUClientRcvdAudioHandler::~SFUClientRcvdAudioHandler()
{

}

gboolean SFUClientRcvdAudioHandler::init()
{
  GError **err = NULL;
  if(m_tpool) {
    XGLOG_ERROR( "SFUClientRcvdAudioHandler::init failed. Thread pool already been initialized !");
    return FALSE;
  }

  m_tpool = g_thread_pool_new((GFunc)rcvd_audio_handler, m_userdata, m_maxThreads, m_exclusive, err);
  if(err && m_tpool == NULL) {
    XGLOG_ERROR("SFUClientRcvdAudioHandler::init failed. Error: %s ", err);
    return FALSE;
  }

  XGLOG_INFO("SFUClientRcvdAudioHandler::init success");

  return TRUE;
}

void SFUClientRcvdAudioHandler::rcvd_audio_handler(gpointer data, gpointer userdata)
{
  Client *srcClient = (Client *)data;
  xGateHmpConference *pHmpConf = (xGateHmpConference *)userdata;

  if(srcClient == NULL) {
    XGLOG_ERROR("Handling received audio buffer failed. Source client is NULL!");
    return;
  }

  string id = srcClient->m_detail.m_callId;

  //dequeue buffer list from rcvd_video_queue
  GstBufferList *list = NULL;
  gpointer glist = NULL;
  GAsyncQueue *rcvd_audio_queue = srcClient->m_rcvd_audio_queue;
  if(rcvd_audio_queue) {
    glist = g_async_queue_try_pop(rcvd_audio_queue);
    if(glist) {
      list = (GstBufferList *)glist;
    }
  }

  if(list == NULL) {
    XGLOG_ERROR("Handling received audio buffer list failed. Received audio buffer list is not valid!");
    return;
  }

	//push received buffer list to mrcp server
	//TODO:Pradeepan,Commented for testing Audio MCU, need to enable with handling decoded packets
  //if(!push_to_mrcp(srcClient, list)) {
	//	XGLOG_ERROR("Pushing audio buffer to mrcp server failed for client: %s", id.c_str());
  //}

	//push received buffer list to recording server
  if(!push_to_record_audio_handler(pHmpConf, srcClient, list)) {
    //XGLOG_ERROR("Pushing audio buffer to record audio handler failed for client: %s", id.c_str());
  }

  //push recevied buffer list to client thread
  if(!push_to_client_thread(srcClient, list)) {
    XGLOG_ERROR("Pushing received audio buffer to client thread failed for client: %s", id.c_str());
  }
  return;
}

gboolean SFUClientRcvdAudioHandler::push_to_mrcp(Client *client, void *vBuffObj)
{
  HmpMRCPVoiceCaptureChannel *pMRCPVoiceChannel = client->m_pHmpMRCPVoiceCaptureChannel;
  HmpMRCPVoiceCaptureBin     *pMRCPVoiceBin = NULL;
  if (pMRCPVoiceChannel)
    pMRCPVoiceBin = pMRCPVoiceChannel->m_pHmpMRCPVoiceCaptureBin;

  if (pMRCPVoiceBin){
    GstBufferList   *list = NULL;
    GstBuffer       *buffer = NULL;

    if (GST_IS_BUFFER_LIST (vBuffObj))
      list = GST_BUFFER_LIST_CAST (vBuffObj);
    else
      buffer = GST_BUFFER_CAST (vBuffObj);

    GstPad* sink_pad = gst_element_get_static_pad(pMRCPVoiceBin->m_mrcp_filter, "sink");

    if (sink_pad){
      if (list){
        GstBufferList *mrcp_buffer_list = gst_buffer_list_copy(list);
        gst_pad_chain_list(sink_pad, mrcp_buffer_list);
      }else if (buffer){
        GstBuffer *mrcp_buffer = gst_buffer_copy(buffer);
        gst_pad_chain(sink_pad, mrcp_buffer);
      }
    }
  }
  return TRUE;
}

gboolean SFUClientRcvdAudioHandler::push_to_record_audio_handler(
                                    xGateHmpConference *hmpConf,
                                    Client *client,
                                    GstBufferList *list)
{
  SFUController *pHmpGstManager = NULL;
  pHmpGstManager = xGateSFUtil::getSFUController();
  GError *err = NULL;
	
  if(hmpConf->m_recorderMap.size() <=0 ) {
    return FALSE;
  }

  SFUClientRecordAudioHandler *pHandler = pHmpGstManager->get_record_audio_handler();
  if(!pHandler) {
    XGLOG_ERROR("Pushing a audio buffer to RecordAudioHandler thread failed. RecordAudioHandler is Null!");
    return FALSE;
  }

  GThreadPool *pHandlerPool = pHandler->get_record_audio_handler_pool();
  if(!pHandlerPool) {
    XGLOG_ERROR("Pushing a audio buffer failed. ReceivedAudioHandler pool is Null!");
    return FALSE;
  }

  //push buffer_list to record audio handler queue
  GAsyncQueue *queue = hmpConf->m_recorder_audio_queue;
  if(queue) {
    gint unprocessed = g_async_queue_length(queue);
    gst_buffer_list_ref (list);
    g_async_queue_push(queue, (gpointer)list);
  }

  g_thread_pool_push(pHandlerPool, client, &err);
  if(err) {
    XGLOG_ERROR("Pushing a record audio buffer to RecorderAudioHandler thread failed. '%s'.", err->message);
    g_error_free(err);
    return FALSE;
  }

  return TRUE;
}

gboolean SFUClientRcvdAudioHandler::push_to_client_thread(Client *client, GstBufferList *list)
{
  HmpFrameBin *frameBin = NULL;
  ACE_Message_Block* pAmb = NULL;
  xGateHmpClientThreadMsg *pMsg = NULL;
  ACE_Time_Value tval ((time(NULL) ));

  frameBin = client->m_FrameChannel->m_pHmpFrameBin;
  
  /* Post message to thread if thread is created , otherwise post the
   * buffer to sender thread. Need to check if RCV thread is required 
   */
  if(client->rcv_data_thrId) {
    pMsg = new xGateHmpClientThreadMsg ();
    //XGLOG_DEBUG("Pushing audio buffer to Client thread");
    tval += ACE_Time_Value(0,1);
    strcpy (pMsg->ucName, "Audio");
    pMsg->m_msgType = XGATE_CLIENT_RCVD_RTP_DATA;
    pMsg->param2 = (void *) (list);
    pMsg->param1 = (void *) (client);
    
    pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
    if(client->rcv_data_msg_q) {
      if(!client->rcv_data_msg_q->enqueue_prio(pAmb, &tval)) {
        XGLOG_ERROR("Failed to push RTP data from padProbe callback to Client thread");
        return FALSE;
      }
    } else {
      XGLOG_ERROR("Failed to push RTP data from padProbe callback rcv_data_msg_q is NULL");
      return FALSE;
    }   
  } else {
    frameBin->on_new_data_rcvd_srcPad(client, "Audio", list);
  }
	return TRUE;
}

GThreadPool *SFUClientRcvdAudioHandler::get_rcvd_audio_handler_pool()
{
  return m_tpool;
}
/* SFUClientRcvdAudioHandler class implmentation - end */

/* SFUClientRecordAudioHandler class implmentation - start */
SFUClientRecordAudioHandler::SFUClientRecordAudioHandler(gpointer userdata, gint maxThreads, gboolean exclusive) :
  SFUClientMediaHandler(userdata, maxThreads, exclusive)
{
  if(maxThreads > MAX_RECORD_AUDIO_HANDLER_THREADS) {
    maxThreads = MAX_RECORD_AUDIO_HANDLER_THREADS;
  }
}

SFUClientRecordAudioHandler::~SFUClientRecordAudioHandler()
{

}

gboolean SFUClientRecordAudioHandler::init()
{
  GError **err;
  if(m_tpool) {
    XGLOG_ERROR( "SFUClientRecordAudioHandler::init failed. Thread pool already been initialized !");
    return FALSE;
  }

  m_tpool = g_thread_pool_new((GFunc)record_audio_handler, m_userdata, m_maxThreads, m_exclusive, err);
  if(err && m_tpool == NULL) {
    XGLOG_ERROR("SFUClientRecordAudioHandler::init failed. Error: %s ", err);
    return FALSE;
  }

  XGLOG_INFO("SFUClientRecordAudioHandler::init success");

  return TRUE;
}

void SFUClientRecordAudioHandler::record_audio_handler(gpointer data, gpointer userdata)
{
  Client *srcClient = (Client *)data;
  xGateHmpConference *pHmpConf = (xGateHmpConference *)userdata;

  if(srcClient == NULL) {
    XGLOG_ERROR("Handling record audio buffer failed. Source client is NULL!");
    return;
  }

  string id = srcClient->m_detail.m_callId;
  XGLOG_TRACE("Received a audio buffer from a client '%s'", id.c_str());

  //dequeue buffer list from rcvd_video_queue
  GstBufferList *list = NULL;
  gpointer glist = NULL;
  GAsyncQueue *queue = pHmpConf->m_recorder_audio_queue;
  if(queue) {
    glist = g_async_queue_try_pop(queue);
    if(glist) {
      list = (GstBufferList *)glist;
    }
  }

  if(list == NULL) {
    XGLOG_ERROR("Handling received audio buffer list failed. Received audio buffer list is not valid!");
    return;
  }
#if 0
  //push received buffer list to recording server
  if(!push_to_recorder(pHmpConf, srcClient, list)) {
    //XGLOG_ERROR("Pushing audio buffer to recording server failed for client: %s", id.c_str());
  }
#endif
  return;
}

#if 0
gboolean SFUClientRecordAudioHandler::push_to_recorder(
                                      xGateHmpConference *hmpConf,
                                      Client *client,
                                      GstBufferList *list)
{
  HmpRecordSendChannel *sendChannel = NULL;

  RECORDER_MAP::iterator itr = hmpConf->m_recorderMap.begin();
  for(itr = hmpConf->m_recorderMap.begin(); itr != hmpConf->m_recorderMap.end(); itr++) {
    sendChannel = itr->second;
    if(sendChannel) {
      HmpRecordAudioSendChannel *audioChannel = sendChannel->m_audioSendChannel;
      if(audioChannel && audioChannel->m_start && audioChannel->m_pHmpRecAudioSendBin) {
        GstPad *sinkPad = audioChannel->m_pHmpRecAudioSendBin->m_udpSinkPad;
        if(!sinkPad){
          XGLOG_ERROR("Audio Sink Pad is NULL for sendchannel:%s",itr->first.c_str());
          continue;
        }
        GstBufferList *rec_buffer = gst_buffer_list_copy_deep(list);
        gst_pad_chain_list(sinkPad, rec_buffer);
      }
    }
  }

  gst_buffer_list_unref(list);
  return TRUE;
}
#endif

gboolean SFUClientRcvdAudioHandler::push_to_recorder(
                                      xGateHmpConference *hmpConf,
                                      Client *client,
                                      void *vBuffObj)
{
  HmpRecordSendChannel *sendChannel = NULL;
  GstBufferList *list = NULL;
  GstBuffer     *buffer = NULL;

  if (GST_IS_BUFFER_LIST (vBuffObj))
    list = GST_BUFFER_LIST_CAST (vBuffObj);
  else
    buffer = GST_BUFFER_CAST (vBuffObj);


  RECORDER_MAP::iterator itr = hmpConf->m_recorderMap.begin();
  for(itr = hmpConf->m_recorderMap.begin(); itr != hmpConf->m_recorderMap.end(); itr++) {
    sendChannel = itr->second;
    if(sendChannel) {
      HmpRecordAudioSendChannel *audioChannel = sendChannel->m_audioSendChannel;
      if(audioChannel && audioChannel->m_start && audioChannel->m_pHmpRecAudioSendBin) {
        GstPad *sinkPad = audioChannel->m_pHmpRecAudioSendBin->m_udpSinkPad;
        if(!sinkPad){
          XGLOG_ERROR("Audio Sink Pad is NULL for sendchannel:%s",itr->first.c_str());
          continue;
        }
        if (list){
          GstBufferList *rec_buffer_list = gst_buffer_list_copy_deep(list);
          /*
           * gst_pad_chain_without_lock should not be called as this buffer is
           * accessed by other client thread and problems of hang are observed
           * when recording is enabled
           */
          gst_pad_chain_list (sinkPad, rec_buffer_list);
        }else if (buffer){
          GstBuffer *rec_buffer = gst_buffer_copy_deep(buffer);
          gst_pad_chain (sinkPad, rec_buffer);
        }
      }
    }
  }

  return TRUE;
}

GThreadPool *SFUClientRecordAudioHandler::get_record_audio_handler_pool()
{
  return m_tpool;
}
/* SFUClientRecordAudioHandler class implmentation - end */
#endif
#include <gst/app/gstappsrc.h>
